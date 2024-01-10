#pragma once

#include <DirectXMath.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace SimpleMath;

const unsigned int MAX_LIGHTS = 12;
const unsigned int DIRECTIONAL_LIGHT = 1;
const unsigned int POINT_LIGHT = 2;
const unsigned int SPOT_LIGHT = 3;


const unsigned int RAST_STATE_COUNT = 3;
const unsigned int RAST_BACK_CULL = 0;
const unsigned int RAST_FRONT_CULL = 1;
const unsigned int RAST_NO_CULL = 2;


const unsigned int BLEND_STATE_COUNT = 2;
const unsigned int BLEND_DISABLED = 0;
const unsigned int BLEND_ENABLED = 1;


const unsigned int BUFFER_COUNT = 3;
const unsigned int DEFAULT_VERTEX_BUFFER = 0;
const unsigned int DEFAULT_PIXEL_BUFFER = 1;
const unsigned int SKYBOX_CONSTANT_BUFFER = 2;


const unsigned int INPUT_LAYOUT_COUNT = 2;
const unsigned int DEFAULT_INPUT_LAYOUT = 0;
const unsigned int SKYBOX_INPUT_LAYOUT = 1;


const unsigned int DEPTH_STATE_COUNT = 2;
const unsigned int SOLID_DEPTH_STATE = 0;
const unsigned int NONE_DEPTH_STATE = 1;


const unsigned int VERTEX_SHADER_COUNT = 2;
const unsigned int DEFAULT_VERTEX_SHADER = 0;
const unsigned int SKYBOX_VERTEX_SHADER = 1;


const unsigned int PIXEL_SHADER_COUNT = 2;
const unsigned int DEFAULT_PIXEL_SHADER = 0;
const unsigned int SKYBOX_PIXEL_SHADER = 1;

struct Transform
{
	Vector3 pos{ 0,0,0 };
	Vector3 rot{ 0,0,0 };
	Vector3 sca{ 1,1,1 };

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