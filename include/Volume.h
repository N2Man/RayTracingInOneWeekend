#ifndef _VOLUME_H_
#define _VOLUME_H_

#include <Hitable.h>

class Volume : public Hitable {
public:
	Volume(Ptr<Hitable> boundary, float density, Ptr<Material> material)
		: boundary(boundary), density(density), material(material){ }

public:
	static const Ptr<Volume> New(Ptr<Hitable> boundary, float density, Ptr<Material> material) {
		return std::make_shared<Volume>(boundary, density, material);
	}

	virtual bool Hit(Ray & ray, HitRecord & rec) const override;

	virtual const Box GetBox() const override { return boundary->GetBox(); }

public:
	Ptr<Hitable> boundary;
	Ptr<Material> material;
	float density; // dP = density * dL
};

bool Volume::Hit(Ray & ray, HitRecord & rec) const {
	Ray dRay(ray.o, ray.d, -Util::LARGE_FLT, Util::LARGE_FLT); // backRay.StartPos() == ray.StartPos()����� backRay.tMin Ҫ��Ϊ����
	HitRecord dRec;

	if (!boundary->Hit(dRay, dRec))
		return false;

	// �ӱ߽���������ߣ�ע�� tMin
	Ray boundRay(ray.o, ray.d, dRay.tMax + Util::DEFAULT_TMIN);
	HitRecord boundRec;
	if (!boundary->Hit(boundRay, boundRec)) // һ�㶼�ǻ��еģ���������жϻ���û��Ҫ
		return false;

	float t0 = std::max<float>(dRay.tMax, ray.tMin); // ��
	float t1 = std::min<float>(boundRay.tMax, ray.tMax); // ��

	float L = (t1 - t0) * boundRay.d.Norm();
	float hitL = -log(1.f - Util::RandF()) / density;
	if (hitL > L)
		return false;

	ray.tMax = t0 + hitL / boundRay.d.Norm();
	rec.p = ray.EndPos();
	// rec.n ����
	rec.material = material;

	return true;
}

#endif // !_VOLUME_H_
