#pragma once

#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include "../GameObjects/Camera.h"
#include "Mesh.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MAX_LIGHTS 12

#define DIRECTIONAL_LIGHT 1
#define POINT_LIGHT 2
#define SPOT_LIGHT 3

using namespace DirectX;

class Camera;
class GameObject;

struct CBUFFER0
{
	XMMATRIX WVP;
	XMVECTOR cameraPos;
	XMVECTOR objectWorldPos;
	XMMATRIX worldMatrix;
	XMMATRIX inverseWorldMatrix;
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

struct PixelCBuffer
{
	Light AmbientLight; // 48 bytes
	Light Lights[MAX_LIGHTS]; // 48 * 12 = 576 bytes
};

class Renderer
{
public:
	static Renderer* Instance;

public:
	Renderer(HINSTANCE instanceHandle, int nCmdShow);
	~Renderer();

	void Draw(std::vector<GameObject*> gameObjects);
	void DrawSkybox();

	ID3D11Device* GetDevice() { return _device; }
	ID3D11DeviceContext* GetDeviceContext() { return _device_context; }

	HWND GetWindowHandle() { return _window_handle; }

	Camera* GetActiveCamera() { return _camera; }
	std::vector<Camera*> GetCameras() { return _cameras; }

private:
	HRESULT InitWindow(HINSTANCE instanceHandle, int nCmdShow);
	HRESULT InitD3D();
	HRESULT InitPipeline();
	void CleanD3D();

private:
	Camera* _camera;

	unsigned int _current_cam = 0;
	std::vector<Camera*> _cameras;

	XMVECTOR _ambient_light_colour = { 0.1f, 0.1f, 0.1f, 0.1f };
	XMVECTOR _directional_light_direction = { 0.2788f, 0.7063f, 0.6506f };
	XMVECTOR _directional_light_colour = { 0.96f, 0.8f, 0.75f, 1.0f };

	XMVECTOR _skybox_ambient_light_colour = { 0.7f, 0.7f, 0.7f, 0.7f };
	XMVECTOR _skybox_directional_light_direction = { 0.2788f, 0.7063f, 0.6506f };
	XMVECTOR _skybox_directional_light_colour = { 0.0f, 0.0f, 0.0f, 0.0f };

	bool Up;

public:
	void SetCamera(Camera* cam) { _camera = cam; }
	Camera* GetCamera() { return _camera; }

	void SwitchCamera();

private:
	const wchar_t* _window_name = L"Horror Game";
	ID3D11Buffer* _const_buffer = NULL;
	ID3D11Buffer* _pixel_const_buffer = NULL;

	HINSTANCE _instance_handle = NULL;
	HWND _window_handle = NULL;

	ID3D11Device* _device = NULL;
	ID3D11DeviceContext* _device_context = NULL;
	IDXGISwapChain* _swapchain = NULL;
	ID3D11InputLayout* _layout = NULL;

	ID3D11RenderTargetView* _back_buffer = NULL;
	ID3D11DepthStencilView* _depth_buffer = NULL;

	ID3D11BlendState* _alpha_blend_enabled = NULL;
	ID3D11BlendState* _alpha_blend_disabled = NULL;

	ID3D11RasterizerState* _rasterizer_no_culling = NULL;
	ID3D11RasterizerState* _rasterizer_back_culling = NULL;
	ID3D11RasterizerState* _rasterizer_front_culling = NULL;

	ID3D11VertexShader* _vertex_shader = NULL;
	ID3D11PixelShader* _pixel_shader = NULL;

	Light AmbientLight;
	Light GameLights[MAX_LIGHTS];
};

