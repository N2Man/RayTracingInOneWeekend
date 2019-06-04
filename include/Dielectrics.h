#ifndef _DIELECTRICS_H_
#define _DIELECTRICS_H_

#include <Material.h>

#include <Util.h>

class Dielectrics :public Material {
public:
	Dielectrics(float ior) :ior(ior) {}

public:
	static Ptr<Dielectrics> New(float ior) {
		return std::make_shared<Dielectrics>(ior);
	}

public:
	virtual const ScatterRst Scatter(const Ray & ray, const HitRecord & rec) const override;

public:
	float ior;
};

// ------------------ ʵ��

const ScatterRst Dielectrics::Scatter(const Ray & ray, const HitRecord & rec) const {
	Vec3f I = ray.d.Normalize();

	Vec3f T;
	if (!Util::Refract(I, rec.n, ior, T)) {
		// ȫ����
		Vec3f dir = Util::Reflect(I, rec.n);
		return ScatterRst(true, { rec.p, dir }, Vec3f(1.f));
	}

	float IoN = I.Dot(rec.n);
	bool isEntering = IoN < 0;
	// �����ǿ����еķ����뷨��ļнǵ�����
	float cosTheta = isEntering ? -I.Dot(rec.n) : T.Dot(rec.n);
	float F = Util::Fresnel_Schlick(ior, cosTheta); // ������ϵ�������������

	if (Util::RandF() < F) {
		// ����
		Vec3f dir = Util::Reflect(I, rec.n);
		return ScatterRst(true, { rec.p, dir }, Vec3f(1.f));
	}

	// ����
	return ScatterRst(true, { rec.p, T }, Vec3f(1.f));
}

#endif // !_DIELECTRICS_H_
