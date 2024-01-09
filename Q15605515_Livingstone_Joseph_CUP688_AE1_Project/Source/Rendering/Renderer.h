#pragma once

#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DDSTextureLoader.h>

#include "GameWindow.h"
#include "../GameObjects/Camera.h"
#include "../GlobalDefinitions.h"
#include "RendererDefinitions.h"
#include "External/text2D.h"

using namespace DirectX;

class Camera;
class GameObject;

class Renderer
{
public:
	static Renderer* Instance;

public:
	Renderer(GameWindow* window);
	~Renderer();

	void Draw(std::vector<GameObject*> gameObjects);
	void DrawSkybox();

	Camera* GetActiveCamera() { return _camera; }
	std::vector<Camera*> GetCameras() { return _cameras; }

	const int& GetFreeLight();

	Light AmbientLight;
	Light GameLights[MAX_LIGHTS];

private:
	HRESULT InitD3D();
	HRESULT InitPipeline();
	void CleanD3D();


	Text2D* _fps_counter;

public:
	void SetCamera(Camera* cam) { _camera = cam; }
	Camera* GetCamera() { return _camera; }
	void SwitchCamera();

	Camera* _camera;
	unsigned int _current_cam = 0;
	std::vector<Camera*> _cameras;

private:
	// Skybox Information
	ModelData m_skybox;
	ID3D11Buffer* m_skybox_vertex_buffer = NULL;
	ID3D11SamplerState* m_skybox_sampler = NULL;
	ID3D11ShaderResourceView* m_skybox_texture = NULL;

public:
	// Core DirectX Functionality Getters.
	GameWindow* GetGameWindow() { return m_window; }
	IDXGISwapChain* GetSwapChain() { return m_swapchain; }
	ID3D11Device* GetDevice() { return m_device; }
	ID3D11DeviceContext* GetDeviceContext() { return m_device_context; }
	ID3D11RenderTargetView* GetBackBuffer() { return m_back_buffer; }
	ID3D11DepthStencilView* GetDepthBuffer() { return m_depth_buffer; }

private:
	// Core DirectX Rendering Info.
	GameWindow* m_window;
	IDXGISwapChain* m_swapchain = NULL;
	ID3D11Device* m_device = NULL;
	ID3D11DeviceContext* m_device_context = NULL;
	ID3D11RenderTargetView* m_back_buffer = NULL;
	ID3D11DepthStencilView* m_depth_buffer = NULL;

public:
	// Stores functionality for loading shaders.
	HRESULT LoadVertexShader(LPCWSTR filename, ID3D11VertexShader** vs, ID3D11InputLayout** il);
	HRESULT LoadPixelShader(LPCWSTR filename, ID3D11PixelShader** ps);

	// DirectX Stores/Settings Getters.
	ID3D11Buffer* GetConstBuffer(const unsigned int& type) { return m_buffers[type]; }
	ID3D11RasterizerState* GetRasterizerState(const unsigned int& type) { return m_rast_states[type]; }
	ID3D11BlendState* GetBlendState(const unsigned int& type) { return m_blend_states[type]; }
	ID3D11InputLayout* GetInputLayout(const unsigned int& type) { return m_layouts[type]; }
	ID3D11DepthStencilState* GetDepthStencilState(const unsigned int& type) { return m_depth_states[type]; }
	ID3D11VertexShader* GetVertexShader(const unsigned int& type) { return m_vertex_shaders[type]; }
	ID3D11PixelShader* GetPixelShader(const unsigned int& type) { return m_pixel_shaders[type]; }

private:
	// Array storage for various different DirectX stores/settings.
	ID3D11Buffer* m_buffers[BUFFER_COUNT] = { NULL, NULL, NULL };
	ID3D11RasterizerState* m_rast_states[RAST_STATE_COUNT] = { NULL, NULL, NULL };
	ID3D11BlendState* m_blend_states[BLEND_STATE_COUNT] = { NULL, NULL };
	ID3D11InputLayout* m_layouts[INPUT_LAYOUT_COUNT] = { NULL, NULL };
	ID3D11DepthStencilState* m_depth_states[DEPTH_STATE_COUNT] = { NULL, NULL };
	ID3D11VertexShader* m_vertex_shaders[VERTEX_SHADER_COUNT] = { NULL, NULL };
	ID3D11PixelShader* m_pixel_shaders[PIXEL_SHADER_COUNT] = { NULL, NULL };

};

