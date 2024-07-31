#pragma once

#include "../../GlobalDefinitions.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>
#include <string>

using namespace DirectX;

struct ShaderData
{
	unsigned int VertexShader = DEFAULT_VERTEX_SHADER;
	unsigned int PixelShader = DEFAULT_PIXEL_SHADER;
	unsigned int InputLayout = DEFAULT_INPUT_LAYOUT;
};

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color = { 0,0,0,0 };
	XMFLOAT2 UV;
	XMFLOAT3 Norm;
};

struct Material
{
	Material() {}

	std::string Name = "";
	int TextureId = -1;
	int Illumination = 1;
	XMFLOAT3 Emissive = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 Ambient = { 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Diffuse = { 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Specular = { 1.0f, 1.0f, 1.0f };
	float SpecularExponent = 0.0f;
	std::string DiffuseMap = "";
};

struct GeometryData
{
	std::vector<XMFLOAT3> vertices;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> uvs;
	std::vector<XMINT3> indices;
	std::vector<std::string> indices_mats;
};

struct MaterialData
{
	// execution index, material name 
	std::unordered_map<int, std::string> PositionUseData;
	std::vector<int> PositionData;
};

struct ModelData
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::string texture;
	MaterialData materials;
};
