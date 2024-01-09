#pragma once

#include <DirectXMath.h>

using namespace DirectX;

#include "GameObject.h"

class Camera : public GameObject
{
public:
    Camera(bool free);
    ~Camera();

	void Update() override {}

	void SetPosition(XMFLOAT3 position) { m_transform.pos = position; }
	void MovePosition(XMINT3 dir);
	void Rotate(XMFLOAT3 dir);

	XMFLOAT3 GetPosition() { return m_transform.pos; }
	float GetPitch() { return m_transform.rot.x; }
	float GetYaw() { return m_transform.rot.y; }
	bool IsFree() { return _free_cam; }

private:
	float _speed = 0.25f;
	bool _free_cam = false;
};

