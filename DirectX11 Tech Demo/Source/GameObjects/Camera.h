#pragma once

#include <DirectXMath.h>

using namespace DirectX;

#include "GameObject.h"

class Camera : public GameObject
{
public:
    Camera(bool free);
    ~Camera();

public:
	void Update() override {}
	void MovePosition(XMINT3 dir);
	void Rotate(XMFLOAT3 dir);

public:
	void SetPosition(XMFLOAT3 position) { m_transform.pos = position; }
	XMFLOAT3 GetPosition() { return m_transform.pos; }
	float GetPitch() { return m_transform.rot.x; }
	float GetYaw() { return m_transform.rot.y; }

	bool IsFree() { return m_free_cam; }

private:
	float m_speed = 0.25f;
	bool m_free_cam = false;
};

