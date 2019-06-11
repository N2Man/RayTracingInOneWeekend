#include <Sphere.h>
#include <HitableList.h>

#include <Lambertian.h>
#include <Metal.h>
#include <Dielectric.h>

#include <Util.h>

#include <ROOT_PATH.h>

#include <fstream>

using namespace std;

const Vec3f Sky(const Ray & ray);

const Vec3f Trace(Ptr<Hitable> scene, Ray & ray, int depth);

int main() {
	int width = 200;
	int height = 100;
	int sampleNum = 100;

	// �������
	Vec3f pos(0.f);
	Vec3f lowerLeft(-2, -1, -1);
	Vec3f horizontal(4, 0, 0);
	Vec3f vertical(0, 2, 0);

	// ����
	auto sphereMid = Sphere::New({ 0, 0, -1 }, 0.5f, Lambertian::New({ 0.1, 0.2, 0.5 }));
	auto sphereLeft = Sphere::New({ -1, 0, -1 }, 0.5f, Dielectric::New(1.5f));
	auto sphereLeftInner = Sphere::New({ -1, 0, -1 }, -0.45f, Dielectric::New(1.5f));
	auto sphereRight = Sphere::New({ 1, 0, -1 }, 0.5f, Metal::New({ 0.8, 0.6, 0.2 }, 0.2f));
	auto ground = Sphere::New({ 0, -100.5, -1 }, 100.f, Lambertian::New({ 0.8, 0.8, 0 }));
	auto scene = HitableList::New({ sphereLeft, sphereLeftInner, sphereMid, sphereRight, ground });

	ofstream rst(ROOT_PATH + "data/09.ppm"); // ppm ��һ�ּ򵥵�ͼƬ��ʽ

	rst << "P3\n" << width << " " << height << "\n255\n";

	for (int j = 0; j < height; j++) { // ��������
		for (int i = 0; i < width; i++) { // ��������
			Vec3f color(0.f);
			for (int k = 0; k < sampleNum; k++) { // ���ز���
				float u = (i + Util::RandF()) / width;
				float v = (height - j + Util::RandF()) / height;

				Vec3f dir = lowerLeft + u * horizontal + v * vertical - pos;
				Ray ray(pos, dir);

				color += Trace(scene, ray, 0);
			}
			color /= float(sampleNum); // ���ֵ
			Vec3f gammaColor = Util::Gamma(color);

			Vec3i iGammaColor = 255.99f * gammaColor;
			rst << iGammaColor.r << " " << iGammaColor.g << " " << iGammaColor.b << endl;
		}
	}

	rst.close();

	return 0;
}

const Vec3f Sky(const Ray & ray) {
	auto normDir = ray.d.Normalize();
	float t = 0.5f * (normDir.y + 1.0f);

	const Vec3f white(1.f);
	const Vec3f blue(0.5, 0.7, 1);

	return Vec3f::Lerp(white, blue, t); // ���Բ�ֵ
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
