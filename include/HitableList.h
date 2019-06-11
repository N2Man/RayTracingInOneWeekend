#ifndef _HITABLE_LIST_H_
#define _HITABLE_LIST_H_

#include <Hitable.h>
#include <vector>

class HitableList : public Hitable {
public:
	HitableList(const std::vector<Ptr<Hitable>> & hitables = std::vector<Ptr<Hitable>>{})
		: hitables(hitables) {}

public:
	// ���� Ptr �ı����ӿ�
	static Ptr<HitableList> New(const std::vector<Ptr<Hitable>> & hitables = std::vector<Ptr<Hitable>>{}) {
		return std::make_shared<HitableList>(hitables);
	}

public:
	virtual bool Hit(Ray & ray, HitRecord & rec) const override;

public:
	void Add(Ptr<Hitable> hitable) { hitables.push_back(hitable); }

private:
	std::vector<Ptr<Hitable>> hitables;
};

// ------------- ʵ��

bool HitableList::Hit(Ray & ray, HitRecord & rec) const {
	bool isHit = false;
	for (auto hitable : hitables) {
		if (hitable->Hit(ray, rec))
			isHit = true;
	}
	return isHit;
}

#endif // !_HITABLE_LIST_H_
