#include "Model.h"
#include "ObjModelLoader.h"

#include "../Renderer.h"
#include "../../GameObjects/GameObject.h"
#include "../../GameObjects/Camera.h"
#include "../GameWindow.h"

Model::Model(const ModelData& model_data)
	: m_owner{ nullptr }, m_model{ model_data }
{
	Renderer* renderer = Renderer::Instance;
	if (!renderer)
		return;

	D3D11_BUFFER_DESC bufferDesc = { 0 };

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * m_model.indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = m_model.indices.data();

	if (FAILED(renderer->GetDevice()->CreateBuffer(&bufferDesc, &initData, &m_index_buffer)))
	{
		OutputDebugString(L"Failed to create index buffer.");
		return;
	}

	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * m_model.vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT result = renderer->GetDevice()->CreateBuffer(&bd, NULL, &m_vertex_buffer);

	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create vertex buffer.");
		return;
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	renderer->GetDeviceContext()->Map(m_vertex_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, m_model.vertices.data(), sizeof(Vertex) * m_model.vertices.size());
	renderer->GetDeviceContext()->Unmap(m_vertex_buffer, NULL);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->GetDevice()->CreateSamplerState(&samplerDesc, &m_sampler);
}

Model::Model(const ModelData& model_data, const ShaderData& shader_data)
	: Model(model_data)
{
	m_shader_data = shader_data;
}

Model::~Model()
{
	if (m_sampler)
		m_sampler->Release();
	if (m_vertex_buffer)
		m_vertex_buffer->Release();
	if (m_index_buffer)
		m_index_buffer->Release();
}

void Model::Draw(Renderer* renderer)
{
	if (!renderer)
		return;

	Camera* camera = renderer->GetCamera();
	GameWindow* window = renderer->GetGameWindow();
	ID3D11DeviceContext* device_context = renderer->GetDeviceContext();
	ID3D11Buffer* vertex_buffer = renderer->GetConstBuffer(DEFAULT_VERTEX_BUFFER);
	ID3D11Buffer* pixel_buffer = renderer->GetConstBuffer(DEFAULT_PIXEL_BUFFER);

	SetBufferDrawData(
		device_context,
		renderer->GetVertexShader(m_shader_data.VertexShader),
		renderer->GetPixelShader(m_shader_data.PixelShader),
		renderer->GetInputLayout(m_shader_data.InputLayout)
	);


	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	device_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
	device_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);
	device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	CBUFFER0 cbuffer;
	XMMATRIX translation, rotation, scale;
	XMMATRIX world, view, projection;

	// View - DO OUTSIDE MODEL
	view = camera->GetViewMatrix();

	// Projection - DO OUTSIDE MODEL
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), window->GetScreenSize()[0] / window->GetScreenSize()[1], 0.01f, 1000.0f);

	// World
	world = m_owner->GetTransform().GetWorldMatrix();

	cbuffer.worldMatrix = world;
	cbuffer.WVP = world * view * projection;
	cbuffer.inverseWorldMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, world));
	XMFLOAT3 position = m_owner->GetTransform().pos;

	cbuffer.objectWorldPos = XMLoadFloat3(&position);
	position = camera->GetPosition();
	cbuffer.cameraPos = XMLoadFloat3(&position);
	XMMATRIX transpose = XMMatrixTranspose(world);


	PixelCBuffer pixelCBuffer;
	pixelCBuffer.AmbientLight = Renderer::Instance->AmbientLight;
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (!Renderer::Instance->GameLights[i].Enabled)
			continue;
		pixelCBuffer.Lights[i].Type = Renderer::Instance->GameLights[i].Type;
		pixelCBuffer.Lights[i].Enabled = Renderer::Instance->GameLights[i].Enabled;
		pixelCBuffer.Lights[i].Strength = Renderer::Instance->GameLights[i].Strength;
		pixelCBuffer.Lights[i].Position = Renderer::Instance->GameLights[i].Position;

		XMVECTOR light_dir = XMVector3Transform(Renderer::Instance->GameLights[i].Direction, transpose);
		pixelCBuffer.Lights[i].Direction = light_dir;

		pixelCBuffer.Lights[i].Colour = Renderer::Instance->GameLights[i].Colour;

		pixelCBuffer.Lights[i].SpotAngle = XMConvertToRadians(Renderer::Instance->GameLights[i].SpotAngle);

	}

	device_context->UpdateSubresource(vertex_buffer, 0, 0, &cbuffer, 0, 0);
	device_context->VSSetConstantBuffers(0, 1, &vertex_buffer);

	for (int i = 0; i < m_model.materials.PositionData.size(); ++i)
	{
		int index = m_model.materials.PositionData[i];
		Material material = ObjModelLoader::GetMaterial(m_model.materials.PositionUseData[index]);


		if (material.Illumination == 6)
		{
			device_context->OMSetBlendState(renderer->GetBlendState(BLEND_ENABLED), 0, 0xffffffff);
			device_context->RSSetState(renderer->GetRasterizerState(RAST_NO_CULL));
		}


		pixelCBuffer.CurrentMaterial.TextureID = material.TextureId;
		pixelCBuffer.CurrentMaterial.Emissive = material.Emissive;
		pixelCBuffer.CurrentMaterial.Ambient = material.Ambient;
		pixelCBuffer.CurrentMaterial.Diffuse = material.Diffuse;
		pixelCBuffer.CurrentMaterial.Specular = material.Specular;
		pixelCBuffer.CurrentMaterial.SpecularExponent = material.SpecularExponent;

		if (material.DiffuseMap.size() > 0)
		{
			ID3D11ShaderResourceView* diffuse_texture = ObjModelLoader::GetTexture(material.DiffuseMap);
			if (diffuse_texture)
				device_context->PSSetShaderResources(0, 1, &diffuse_texture);
		}

		device_context->UpdateSubresource(pixel_buffer, 0, 0, &pixelCBuffer, 0, 0);
		device_context->PSSetConstantBuffers(0, 1, &pixel_buffer);

		int end = i + 1;
		if (end >= m_model.materials.PositionData.size())
			end = m_model.vertices.size();
		else
			end = m_model.materials.PositionData[end];
		end = end - (index);

		device_context->Draw(end, index);

		if (material.Illumination == 6)
		{
			device_context->OMSetBlendState(renderer->GetBlendState(BLEND_DISABLED), 0, 0xffffffff);
			device_context->RSSetState(renderer->GetRasterizerState(RAST_BACK_CULL));
		}
	}
}

void Model::SetBufferDrawData
   (ID3D11DeviceContext* device_context, 
	ID3D11VertexShader* vertex_shader,
	ID3D11PixelShader* pixel_shader,
	ID3D11InputLayout* input_layout)
{
	device_context->VSSetShader(vertex_shader, 0, 0);
	device_context->PSSetShader(pixel_shader, 0, 0);
	device_context->IASetInputLayout(input_layout);

	ID3D11ShaderResourceView* texture = ObjModelLoader::GetTexture(m_model.texture);
	device_context->PSSetShaderResources(0, 1, &texture);
	device_context->PSSetSamplers(0, 1, &m_sampler);
}