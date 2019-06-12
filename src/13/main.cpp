#include <Sphere.h>
#include <HitableList.h>

#include <Lambertian.h>
#include <Metal.h>
#include <Dielectric.h>

#include <Util.h>

#include <Camera.h>

#include <ROOT_PATH.h>

#include <fstream>
#include <thread>
#include <array>

using namespace std;

const Vec3f Sky(const Ray & ray);
const Vec3f Trace(Ptr<Hitable> scene, Ray & ray, int depth);
Ptr<Hitable> GenScene();
void SaveImg(const vector<vector<Vec3f>> & img);

int main() {
	int width = 200;
	int height = 100;
	int sampleNum = 32;
#ifdef NDEBUG // release
	int cpuNum = Util::NumCPU();
#else
	int cpuNum = 1;
#endif // NDEBUG
	printf("CPU : %d\n", cpuNum);

	// 相机参数
	Vec3f pos(13, 2, 3);
	Vec3f target(0, 0, 0);
	float focusDist = 10.0;
	float vfov = 20.f;
	float aspect = float(width) / float(height);
	float aperture = 0.1f;
	Camera camera(pos, target, vfov, aspect, aperture, focusDist);

	// 场景
	auto scene = GenScene();

	vector<vector<Vec3f>> img(height, vector<Vec3f>(width)); // 先行后列

	vector<thread> workers;
	vector<int> pixelNums(cpuNum, 0);
	for (int id = 0; id < cpuNum; id++) {
		workers.push_back(thread([=, &img, &pixelNums]() {
			for (int idx = id; idx < width*height; idx += cpuNum) {
				int y = idx / width;
				int x = idx - y * width;
				
				Vec3f color(0);
				for (int k = 0; k < sampleNum; k++) {
					float u = (x + Util::RandF()) / width;
					float v = (height - y + Util::RandF()) / height;

					Ray ray = camera.GenRay(u, v);

					color += Trace(scene, ray, 0);
				}
				img[y][x] = color / (float)sampleNum;

				pixelNums[id]++;

				int sum = 0;
				for (auto pixelNum : pixelNums)
					sum += pixelNum;
				float rate = sum / float(width*height);
				printf("\r%.2f%% ...", rate*100.f);
			}
		}));
	}
	for (auto & worker : workers) // 主线程等待所有子线程完成任务
		worker.join();

	printf("\n"
		"Evaluate done\n"
		"Save image...\n");

	SaveImg(img);

	return 0;
}

const Vec3f Sky(const Ray & ray) {
	auto normDir = ray.d.Normalize();
	float t = 0.5f * (normDir.y + 1.0f);

	const Vec3f white(1.f);
	const Vec3f blue(0.5, 0.7, 1);

	return Vec3f::Lerp(white, blue, t); // 线性插值
}

const Vec3f Trace(Ptr<Hitable> scene, Ray & ray, int depth) {
	HitRecord rec;
	if (scene->Hit(ray, rec)) {
		if (depth >= 50)
			return Vec3f(0.f);

		auto scatterRst = rec.material->Scatter(ray, rec);
		if (!scatterRst.isScatter)
			return Vec3f(0.f);

		return scatterRst.attenuation * Trace(scene, scatterRst.ray, depth + 1);
	}

	return Sky(ray);
}

Ptr<Hitable> GenScene() {
	int n = 500;
	auto scene = HitableList::New();

	auto ground = Sphere::New({ 0, -1000, 0 }, 1000.f, Lambertian::New(Vec3f{ 0.5f }));
	scene->push_back(ground);

	int i = 1;
	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			float choose_mat = Util::RandF();
			Vec3f center(a + 0.9f*Util::RandF(), 0.2f, b + 0.9f*Util::RandF());
			if ((center - Vec3f(4, 0.2, 0)).Norm() > 0.9f) {
				Ptr<Material> material;
				if (choose_mat < 0.8f) // diffuse
					material = Lambertian::New({ Util::RandF()*Util::RandF(), Util::RandF()*Util::RandF(), Util::RandF()*Util::RandF() });
				else if (choose_mat < 0.95f) // metal
					material = Metal::New({ 0.5f*(1 + Util::RandF()), 0.5f*(1.f + Util::RandF()), 0.5f*(1.f + Util::RandF()) }, 0.5f*Util::RandF());
				else // glass
					material = Dielectric::New(1.5f);

				scene->push_back(Sphere::New(center, 0.2f, material));
			}
		}
	}

	scene->push_back(Sphere::New({ 0, 1, 0 }, 1.f, Dielectric::New(1.5f)));
	scene->push_back(Sphere::New({ -4, 1, 0 }, 1.f, Lambertian::New({ 0.4, 0.2, 0.1 })));
	scene->push_back(Sphere::New({ 4, 1, 0 }, 1.f, Metal::New({ 0.7, 0.6, 0.5 }, 0.f)));

	return scene;
}

void SaveImg(const vector<vector<Vec3f>> & img) {
	int width = img.front().size();
	int height = img.size();

	ofstream rst(ROOT_PATH + "data/13.ppm"); // ppm 是一种简单的图片格式

	rst << "P3\n" << width << " " << height << "\n255\n";

	for (int j = 0; j < height; j++) { // 从上至下
		for (int i = 0; i < width; i++) { // 从左至右

			Vec3f gammaColor = Util::Gamma(img[j][i]);

			Vec3i iGammaColor = 255.99f * gammaColor;
			rst << iGammaColor.r << " " << iGammaColor.g << " " << iGammaColor.b << endl;
		}
	}

	rst.close();
}
