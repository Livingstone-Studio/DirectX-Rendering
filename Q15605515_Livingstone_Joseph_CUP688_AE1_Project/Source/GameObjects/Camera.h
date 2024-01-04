#pragma once

#include <DirectXMath.h>

using namespace DirectX;

#include "GameObject.h"

class Camera :
    public GameObject
{
public:
    Camera(bool free);
    ~Camera();

	void MovePosition(XMINT3 dir);
	void Rotate(XMFLOAT3 dir);

	XMFLOAT3 GetPosition() { return _transform.pos; }
	float GetPitch() { return _transform.rot.x; }
	float GetYaw() { return _transform.rot.y; }
	bool IsFree() { return _free_cam; }

	XMMATRIX GetViewMatrix();

private:
	float _speed = 0.001f;
	bool _free_cam = false;
};

