#pragma once

#include <DirectXMath.h>

using namespace DirectX;

#include "GameObject.h"

class Camera :
    public GameObject
{
public:
    Camera();
    ~Camera();

	void MovePosition(XMINT3 dir);
	void Rotate(XMFLOAT3 dir);

	XMFLOAT3 GetPosition() { return _transform.pos; }
	float GetPitch() { return _transform.rot.x; }
	float GetYaw() { return _transform.rot.y; }

	XMMATRIX GetViewMatrix();

	GameObject* GetSkybox() { return _skybox; }

private:
	//XMFLOAT3 _position;
	//float _pitch = XM_PIDIV2;
	//float _yaw = 0;

	float _speed = 0.001f;

	GameObject* _skybox;

};

