#pragma once

#include "ModelDataStructs.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <unordered_map>
#include <string>

class Renderer;
class GameWindow;
class GameObject;
class Camera;

class Model
{
public:
	Model(const ModelData& model_data);
	Model(const ModelData& model_data, const ShaderData& shader_data);
	~Model();

public:
	void Draw(Renderer* renderer);

public:
	const ModelData& GetModelData() { return m_model; }
	void SetOwner(GameObject* owner) { m_owner = owner; }
	void SetShaderData(const ShaderData& shader_data) { m_shader_data = shader_data; }

private:
	GameObject* m_owner;
	ModelData m_model;
	ShaderData m_shader_data;

private:
	void SetBufferDrawData(ID3D11DeviceContext* device_context,
		ID3D11VertexShader* vertex_shader,
		ID3D11PixelShader* pixel_shader,
		ID3D11InputLayout* input_layout);
	ID3D11Buffer* GetVertexBuffer() { return m_vertex_buffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_index_buffer; }

private:
	ID3D11Buffer* m_vertex_buffer = NULL;
	ID3D11Buffer* m_index_buffer = NULL;
	ID3D11SamplerState* m_sampler = NULL;

};