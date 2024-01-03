#include "Camera.h"


Camera::Camera(bool free)
	: GameObject(L"Assets/camera.obj", L"Assets/MissingTexture.png", false, { {0, 0, 0}, {XM_PIDIV2,0,0}, {0.5f,0.5f,0.5f} })
{
	_free_cam = free;

	if (!_free_cam)
	{
		SetPosition({0.0f, 0.5f, 0.0f});
		_transform.rot = { XM_PI - XM_PIDIV4, 0.0f, 0.0f };
		_speed = 0.005f;
	}
}

Camera::~Camera()
{
}

void Camera::MovePosition(XMINT3 dir)
{
	if (_free_cam)
	{
		_transform.pos = {
			(float)(_transform.pos.x + (_speed * dir.z) * sin(_transform.rot.y) * sin(_transform.rot.x)),
			(float)(_transform.pos.y + (_speed * dir.z) * cos(_transform.rot.x)),
			(float)(_transform.pos.z + (_speed * dir.z) * cos(_transform.rot.y) * sin(_transform.rot.x))
		};

		_transform.pos = {
		(float)(_transform.pos.x + (_speed * dir.x) * cos(_transform.rot.y)),
		_transform.pos.y,
		(float)(_transform.pos.z + (_speed * dir.x) * (-sin(_transform.rot.y)))
		};
		return;
	}

	_transform.pos = {
		_transform.pos.x + (_speed * dir.x),
		_transform.pos.y,
		_transform.pos.z + (_speed * dir.z)
	};
}

void Camera::Rotate(XMFLOAT3 dir)
{
	if (_free_cam)
	{
		if (_transform.rot.x + dir.x > 0.0f && _transform.rot.x + dir.x < XM_PI)
			_transform.rot.x += dir.x;

		if (_transform.rot.x < 0)
			_transform.rot.x = 0;
		else if (_transform.rot.x > XM_PI)
			_transform.rot.x = 0;

		_transform.rot.y += dir.y;
		_transform.rot.z += dir.z;
		return;
	}


}

XMMATRIX Camera::GetViewMatrix()
{
	XMVECTOR eyePos = { _transform.pos.x, _transform.pos.y, _transform.pos.z };
	XMVECTOR camUp = { 0,1,0 };
	XMVECTOR lookAt = { sin(_transform.rot.y) * sin(_transform.rot.x),
						cos(_transform.rot.x),
						cos(_transform.rot.y) * sin(_transform.rot.x) };
	XMMATRIX view = XMMatrixLookToLH(eyePos, lookAt, camUp);
	return view;
}