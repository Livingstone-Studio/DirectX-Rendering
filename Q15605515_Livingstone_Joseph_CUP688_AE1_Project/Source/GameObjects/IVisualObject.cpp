#include "IVisualObject.h"

#include "../Rendering/Renderer.h"
#include "../Core/AssetManager.h"

IVisualObject::IVisualObject()
	: IVisualObject(L"Assets/cube.obj", L"Assets/MissingTexture.png")
{
}

IVisualObject::IVisualObject(const wchar_t* meshPath, const wchar_t* texturePath, bool transpa)
	: _mesh{}, Transparency{ transpa }
{
	Renderer* renderer = Renderer::Instance;
	if (!renderer)
		return;

	if (meshPath == L"" || texturePath == L"")
	{
		_mesh = AssetManager::LoadObj(L"Assets/cube.obj");
		texturePath = L"Assets/MissingTexture.png";		
		OutputDebugString(L"Invalid object/texture path into IVisualObject.");
	}
	else 
	{
		_mesh = AssetManager::LoadObj(meshPath);
	}

	D3D11_BUFFER_DESC bufferDesc = { 0 };

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * _mesh.Indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = _mesh.Indices.data();

	if (FAILED(renderer->GetDevice()->CreateBuffer(&bufferDesc, &initData, &_index_buffer)))
	{
		OutputDebugString(L"Failed to create index buffer.");
		return;
	}

	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * _mesh.Vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT result = renderer->GetDevice()->CreateBuffer(&bd, NULL, &_vertex_buffer);

	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create vertex buffer.");
		return;
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	renderer->GetDeviceContext()->Map(_vertex_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, _mesh.Vertices.data(), sizeof(Vertex) * _mesh.Vertices.size());
	renderer->GetDeviceContext()->Unmap(_vertex_buffer, NULL);

	result = DirectX::CreateWICTextureFromFile(renderer->GetDevice(), renderer->GetDeviceContext(), texturePath, NULL, &_texture);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create texture.");
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->GetDevice()->CreateSamplerState(&samplerDesc, &_sampler);
}

IVisualObject::~IVisualObject()
{
	if (_vertex_buffer)
		_vertex_buffer->Release();
	if (_index_buffer)
		_index_buffer->Release();
	if (_texture)
		_texture->Release();
	if (_sampler)
		_sampler->Release();
}

const Mesh& IVisualObject::CubeMesh()
{
	return Mesh({
			{XMFLOAT3{-0.5f, -0.5f, -0.5f}, XMFLOAT4{1.0f,  0.0f,  0.0f,  1.0f}, XMFLOAT2{0.0f, 1.0f}, XMFLOAT3{-0.5773f, -0.5773f, -0.5773f}},  // Front BL
			{XMFLOAT3{-0.5f,  0.5f, -0.5f}, XMFLOAT4{0.0f,  1.0f,  0.0f,  1.0f}, XMFLOAT2{0.0f, 0.0f}, XMFLOAT3{-0.5773f,  0.5773f, -0.5773f}},  // Front TL
			{XMFLOAT3{ 0.5f,  0.5f, -0.5f}, XMFLOAT4{0.0f,  0.0f,  1.0f,  1.0f}, XMFLOAT2{1.0f, 0.0f}, XMFLOAT3{ 0.5773f,  0.5773f, -0.5773f}},  // Front TR
			{XMFLOAT3{ 0.5f, -0.5f, -0.5f}, XMFLOAT4{1.0f,  1.0f,  1.0f,  1.0f}, XMFLOAT2{1.0f, 1.0f}, XMFLOAT3{ 0.5773f, -0.5773f, -0.5773f}},  // Front BR

			{XMFLOAT3{-0.5f, -0.5f,  0.5f}, XMFLOAT4{0.0f,  1.0f,  1.0f,  1.0f}, XMFLOAT2{0.0f, 1.0f}, XMFLOAT3{-0.5773f, -0.5773f, 0.5773f}},  // Back BL
			{XMFLOAT3{-0.5f,  0.5f,  0.5f}, XMFLOAT4{1.0f,  0.0f,  1.0f,  1.0f}, XMFLOAT2{0.0f, 0.0f}, XMFLOAT3{-0.5773f,  0.5773f, 0.5773f}},  // Back TL
			{XMFLOAT3{ 0.5f,  0.5f,  0.5f}, XMFLOAT4{1.0f,  1.0f,  0.0f,  1.0f}, XMFLOAT2{1.0f, 0.0f}, XMFLOAT3{ 0.5773f,  0.5773f, 0.5773f}},  // Back TR
			{XMFLOAT3{ 0.5f, -0.5f,  0.5f}, XMFLOAT4{0.0f,  0.0f,  0.0f,  1.0f}, XMFLOAT2{1.0f, 1.0f}, XMFLOAT3{ 0.5773f, -0.5773f, 0.5773f}},  // Back BR
		},
		{
			0,1,2,2,3,0,		7,6,5,5,4,7,		4,5,1,1,0,4,
			3,2,6,6,7,3,		1,5,6,6,2,1,		4,0,3,3,7,4
		});
}
