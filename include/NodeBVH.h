#ifndef _NODE_BVH_H_
#define _NODE_BVH_H_

#include <Box.h>
#include <Hitable.h>
#include <vector>

class NodeBVH : public Hitable{
public:
	NodeBVH(Ptr<Hitable> left, Ptr<Hitable> right)
		: left(left), right(right), box(Box::Union(left->GetBox(), right->GetBox())) { }
	NodeBVH(Ptr<Hitable> hitable) : NodeBVH(hitable, hitable) { }

public:
	static const Ptr<NodeBVH> New(Ptr<Hitable> left, Ptr<Hitable> right) {
		return std::make_shared<NodeBVH>(left, right);
	}
	static const Ptr<NodeBVH> New(Ptr<Hitable> hitable) {
		return std::make_shared<NodeBVH>(hitable);
	}

	static const Ptr<NodeBVH> Build(const std::vector<Ptr<Hitable>> & hitables);

public:
	virtual bool Hit(Ray & ray, HitRecord & rec) const override;

	virtual const Box GetBox() const override { return box; }

private:
	bool IsLeaf() const { return left == right; }

public:
	Ptr<Hitable> left;
	Ptr<Hitable> right;
	Box box;
};

bool NodeBVH::Hit(Ray & ray, HitRecord & rec) const {
	if (IsLeaf())
		return left->Hit(ray, rec);

	auto leftRst = left->GetBox().Hit(ray);
	auto rightRst = right->GetBox().Hit(ray);

	if (leftRst.hit) { // ����������
		if (rightRst.hit) { // �����Ҳ����
			if (leftRst.t0 < rightRst.t0) { // ��ڵ㿿ǰ
				bool hit = left->Hit(ray, rec);
				if (!hit) // ��ڵ�δ���У����ֻཻȡ�����ҽڵ�
					return right->Hit(ray, rec);

				// �ѻ������㣬��������յ��ܹ����Ҳ���ӣ���������ҽڵ�����ж�
				if (ray.tMax > rightRst.t0)
					right->Hit(ray, rec);

				// ǰ����ȷ�����нڵ㱻������
				return true;
			}
			else {
				// ���ϱߵ���������෴
				bool hit = right->Hit(ray, rec);
				if (!hit)
					return left->Hit(ray, rec);

				if (ray.tMax > leftRst.t0)
					left->Hit(ray, rec);

				return true;
			}
		}
		else // δ�����Ҳ���ӣ����ֻཻȡ������ڵ�
			return left->Hit(ray, rec);
	}
	else if (rightRst.hit) // δ�����Ҳ���ӣ����ֻཻȡ�����ҽڵ�
		return right->Hit(ray, rec);
	else
		return false;
}

const Ptr<NodeBVH> NodeBVH::Build(const std::vector<Ptr<Hitable>> & hitables) {
	if (hitables.size() == 1) // �ݹ���ֹ����
		return NodeBVH::New(hitables.front());

	// ��ȡ��Χ�����ģ������¼
	std::vector<float> centers[3];
	for (auto hitable : hitables){
		auto center = hitable->GetBox().Center();
		for (int axis = 0; axis < 3; axis++)
			centers[axis].push_back(center[axis]);
	}
	
	// �������ĵķ���
	float vars[3];
	for (int axis = 0; axis < 3; axis++)
		vars[axis] = Util::Var(centers[axis]);

	// ���������ἴΪ�����ᣬ����ֵΪ��ֵ
	int spiltAxis = (vars[0] > vars[1] && vars[0] > vars[2]) ? 0 : (vars[1] > vars[2] ? 1 : 2);
	float spiltVal = Util::Mean(centers[spiltAxis]);

	// ���ݻ�����ͻ���ֵ���л���
	std::vector<Ptr<Hitable>> leftHitables;
	std::vector<Ptr<Hitable>> rightHitables;
	for (auto hitable : hitables) {
		auto center = hitable->GetBox().Center();
		if (center[spiltAxis] < spiltVal)
			leftHitables.push_back(hitable);
		else
			rightHitables.push_back(hitable);
	}
	
	// ��ʱ�Ữ��ʧ�ܣ������غϣ�����ʱ��ǿ�о���
	if (leftHitables.size() == 0 || rightHitables.size() == 0) {
		leftHitables.clear();
		rightHitables.clear();

		auto halfNum = hitables.size() / 2;
		for (size_t i = 0; i < halfNum; i++)
			leftHitables.push_back(hitables[i]);
		for (size_t i = halfNum; i < hitables.size(); i++)
			rightHitables.push_back(hitables[i]);
	}
	
	// �ݹ�
	auto leftNode = NodeBVH::Build(leftHitables);
	auto rightNode = NodeBVH::Build(rightHitables);
	return NodeBVH::New(leftNode, rightNode);
}

#endif // !_NODE_BVH_H_
