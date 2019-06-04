#ifndef _HITABLE_H_
#define _HITABLE_H_

#include <Ray.h>

#include <memory>

template<typename T>
using Ptr = std::shared_ptr<T>;

struct HitRecord {
	// float t; ������ Ray �м�¼ tMin �� tMax
	Vec3f p;
	Vec3f n;
};

class Hitable {
public:
	// ��Ϊ Ray �к��� tMin �� tMax���������ﲻ��Ҫ������Ӧ����
	// ����ཻ���������޸� ray �� tMax
	virtual bool Hit(Ray & ray, HitRecord & rec) const = 0;
};

#endif // !_HITABLE_H_
