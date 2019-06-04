#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <Vec3.h>
#include <Ray.h>
#include <Util.h>

class Camera {
public:
	// vfov: verticle field of view������ĽǶ�
	// aspect: width / height����߱�
	// aperture: ͸��ֱ��
	// focusDis: �Խ����룬������������������ľ���
	Camera(const Vec3f & pos, const Vec3f & target, float vfov, float aspect, 
	// �� 11 ������
	float aperture = 0.f, float focusDis = 1.f );

public:
	const Ray GenRay(float u, float v) const;

private:
	Vec3f pos; // ���λ��
	Vec3f lowerLeft; // ����ƽ������½�
	Vec3f horizontal; // ����Ϊ����ƽ����
	Vec3f vertical; // ����Ϊ����ƽ��߶�

	// �� 11 ������
	float lensRadius; // ͸���뾶
	Vec3f right; // ��λ����
	Vec3f up; // ��λ����
};

// -------------- ʵ��
Camera::Camera(const Vec3f & pos, const Vec3f & target, float vfov, float aspect,
	// �� 11 ������
	float aperture, float focusDis)
	: pos(pos), lensRadius(aperture / 2.0f)
{
	float theta = vfov / 180.f * Util::PI; // �Ƕ�ת����
	float halfHeight = focusDis * tan(theta / 2.f); // ����ƽ��߶ȵ�һ��
	float halfWidth = aspect * halfHeight; // ����ƽ���ȵ�һ��

	Vec3f front = (target - pos).Normalize();
	const Vec3f worldUp(0, 1, 0);
	right = front.Cross(worldUp);
	up = right.Cross(front);

	lowerLeft = pos + focusDis * front - halfWidth * right - halfHeight * up;
	horizontal = 2 * halfWidth * right;
	vertical = 2 * halfHeight * up;
}

const Ray Camera::GenRay(float u, float v) const {
	// �� lensRadius == 0 ʱ�˻�Ϊ
	// {pos, lowerLeft + u * horizontal + v * vertical - pos}

	Vec3f target = lowerLeft + u * horizontal + v * vertical;

	Vec3f rd = lensRadius * Util::RandInDisk();
	Vec3f offset = right * rd.x + up * rd.y;
	Vec3f origin = pos + offset;

	return { origin,  target - origin };
}

#endif // !_CAMERA_H_
