#pragma once

#include <DirectXMath.h>
#include <vector>

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

	bool IsValid() { return (Vertices.size() != 0 && Indices.size() != 0); }
};