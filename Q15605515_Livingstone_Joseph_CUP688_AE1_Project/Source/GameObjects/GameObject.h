#pragma once

#include "IVisualObject.h"

struct Transform
{
	XMFLOAT3 pos{ 0,0,0 };
	XMFLOAT3 rot{ 0,0,0 };
	XMFLOAT3 sca{ 1,1,1 };

	Transform() : pos{ 0,0,0 }, rot{ 0,0,0 }, sca{ 1,1,1 } {};
	Transform(XMFLOAT3 p, XMFLOAT3 r, XMFLOAT3 s) : pos{ p }, rot{ r }, sca{ s } {};

	XMFLOAT3 GetForward()
	{
		XMFLOAT3 forward = { (float)sin(rot.y) * (float)sin(rot.x),
			(float)cos(rot.x),
			(float)cos(rot.y) * (float)sin(rot.x) };
		return forward;
	}

	XMFLOAT3 GetBackward()
	{
		XMFLOAT3 forward = { (float)sin(rot.y) * (float)sin(rot.x) * -1,
			(float)cos(rot.x) * -1,
			(float)cos(rot.y) * (float)sin(rot.x) * -1 };
		return forward;
	}

	XMMATRIX GetWorldMatrix()
	{
		XMMATRIX translation = XMMatrixTranslation(pos.x, pos.y, pos.z);
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		XMMATRIX scale = XMMatrixScaling(sca.x, sca.y, sca.z);
		XMMATRIX world = scale * rotation * translation;
		return world;
	}
};

class GameObject : public IVisualObject
{
public :
	GameObject(Transform t);
	GameObject(const wchar_t* meshPath, const wchar_t* texturePath, bool transpa, Transform t);
	~GameObject();

	Transform GetTransform() {	return _transform;	};
	void SetTransform(Transform t) { _transform = t; }
	void SetPosition(XMFLOAT3 p) { _transform.pos = p; }
	void Rotate(XMFLOAT3 r) { _transform.rot = XMFLOAT3(_transform.rot.x + r.x,
														_transform.rot.y + r.y, 
														_transform.rot.z + r.z ); }


protected:
	Transform _transform;
};
