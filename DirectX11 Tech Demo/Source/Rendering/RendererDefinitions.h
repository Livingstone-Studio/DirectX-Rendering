#pragma once

#include <DirectXMath.h>
#include <DirectXColors.h>

using namespace DirectX;

struct CBUFFER0
{
	XMMATRIX WVP;
	XMVECTOR cameraPos;
	XMVECTOR objectWorldPos;
	XMMATRIX worldMatrix;
	XMMATRIX inverseWorldMatrix;
};

struct SKYBOXCBUFFER0
{
	XMMATRIX WVP;
};

struct Light
{
	// 16 bytes
	int Type = 0; // 4 bytes
	bool Enabled = false; // 4 bytes
	float Strength = 0; // 4 bytes
	float SpotAngle = 0; // 4 bytes

	// 16 bytes
	XMVECTOR Position = { 0,0,0,0 }; // 16 bytes

	// 16 bytes
	XMVECTOR Direction = { 0,0,0,0 }; // 16 bytes

	// 16 bytes
	XMVECTOR Colour = Colors::White; // 16 bytes
};

struct MaterialBuffer
{
	// 16 bytes
	float TextureID = -1; // 4 bytes
	XMFLOAT3 Ambient = { 0.0f, 0.0f, 0.0f }; // 12 bytes

	// 16 bytes
	XMFLOAT3 Diffuse = { 0.0f, 0.0f, 0.0f }; // 12 bytes
	float padding1 = 0.0f; // 4 bytes

	// 16 bytes
	XMFLOAT3 Specular = { 0.0f, 0.0f, 0.0f }; // 12 bytes
	float SpecularExponent = 0.0f; // 4 bytes

	// 16 bytes
	XMFLOAT3 Emissive = { 0.0f, 0.0f, 0.0f }; // 12 bytes
	float padding2 = 0.0f; // 4 bytes
};

struct PixelCBuffer
{
	Light AmbientLight; // 48 bytes
	Light Lights[MAX_LIGHTS]; // 48 * 12 = 576 bytes

	MaterialBuffer CurrentMaterial; // 64 bytes
};