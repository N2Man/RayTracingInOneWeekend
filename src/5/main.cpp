#include <Sphere.h>
#include <HitableList.h>

#include <ROOT_PATH.h>

#include <fstream>

using namespace std;

const Vec3f Sky(const Ray & ray) {
	auto normDir = ray.d.Normalize();
	float t = 0.5f * (normDir.y + 1.0f);

	const Vec3f white(1.f);
	const Vec3f blue(0.5, 0.7, 1);
	
	return Vec3f::Lerp(white, blue, t);
}

const Vec3f Trace(Ptr<Hitable> scene, Ray & ray) {
	HitRecord rec;
	if (scene->Hit(ray, rec))
		return 0.5f * (rec.n + Vec3f(1.f)); // ӳ�䵽 0 - 1

	return Sky(ray);
}

int main() {
	int width = 200;
	int height = 100;

	// �������
	Vec3f pos(0.f);
	Vec3f lowerLeft(-2, -1, -1);
	Vec3f horizontal(4, 0, 0);
	Vec3f vertical(0, 2, 0);

	// ����
	auto sphere = Sphere::New({ 0, 0, -1 }, 0.5f);
	auto ground = Sphere::New({ 0, -100.5, -1 }, 100.f);
	auto scene = HitableList::New({ sphere,ground });

	ofstream rst(ROOT_PATH + "data/5.ppm"); // ppm ��һ�ּ򵥵�ͼƬ��ʽ

	rst << "P3\n" << width << " " << height << "\n255\n";

	for (int j = 0; j < height; j++) { // ��������
		for (int i = 0; i < width; i++) { // ��������
			float u = float(i) / float(width);
			float v = float(height - j) / float(height);

			Ray ray(pos, lowerLeft + u * horizontal + v * vertical);

			auto color = Trace(scene, ray);

			Vec3i iColor = 255.99f * color;
			rst << iColor.r << " " << iColor.g << " " << iColor.b << endl;
		}
	}

	rst.close();

	return 0;
}
