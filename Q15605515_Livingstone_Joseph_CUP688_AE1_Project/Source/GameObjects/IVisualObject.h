#pragma once

#include <d3d11.h>
#include <WICTextureLoader.h>
#include "../Rendering/Mesh.h"

class IVisualObject
{
public:
	IVisualObject();
	IVisualObject(const wchar_t* meshPath, const wchar_t* texturePath, bool transpa = false);
	~IVisualObject();

	ID3D11Buffer* const* GetVertexBuffer() { return &_vertex_buffer; }
	ID3D11Buffer* GetIndexBuffer() { return _index_buffer; }

	ID3D11ShaderResourceView* const* GetTexture() { return &_texture; }
	ID3D11SamplerState* const* GetSampler() { return &_sampler; }

	const Mesh& GetMesh() { return _mesh; }

	const Mesh& CubeMesh();

	bool Transparency = false;

private:
	Mesh _mesh;

	ID3D11Buffer* _vertex_buffer = NULL;
	ID3D11Buffer* _index_buffer = NULL;
	ID3D11ShaderResourceView* _texture = NULL;
	ID3D11SamplerState* _sampler = NULL;
};

