#pragma once

#include <DirectXMath.h>
#include <vector>
#include <string>
#include <map>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT2 UV;
	XMFLOAT3 Norm;
};

struct Mesh 
{
	Mesh() {}
	Mesh(std::vector<Vertex> verts, std::vector<unsigned int> inds) : Vertices{ verts }, Indices{ inds } {}
	std::vector<Vertex> Vertices;
	std::vector<unsigned int> Indices;

	std::map<int, int> Materials; // key - int index for mat indexs, value - the index of mat in asset manager
	std::vector<int> MaterialIndexs; // when the mesh needs to change to next mat.

	bool IsValid() { return (Vertices.size() != 0 && Indices.size() != 0); }
};

struct Material
{
	int Id = -1;
	int Illumination = 1;
	XMFLOAT3 Emissive = { 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Ambient = { 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Diffuse = { 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Specular = { 1.0f, 1.0f, 1.0f };
	float SpecularExponent = 0.0f;

	std::string DiffuseMap = "";
};

struct MaterialBuffer
{
	int Id = -1;
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