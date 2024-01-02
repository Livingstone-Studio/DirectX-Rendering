#include "Renderer.h"

#include "../Core/Input.h"
#include "../Rendering/External/ReadData.h"
#include "../GameObjects/GameObject.h"
#include "../GameObjects/Camera.h"
#include "../GameObjects/IVisualObject.h"
#include <chrono>
#include <iostream>
Renderer* Renderer::Instance{ nullptr };

Renderer::Renderer(HINSTANCE instanceHandle, int nCmdShow)
{
	if (FAILED(InitWindow(instanceHandle, nCmdShow)))
	{
		MessageBox(NULL, L"Failed to create window.", L"Critical Error!", MB_ICONERROR | MB_OK);
		CleanD3D();
		return;
	}
	if (FAILED(InitD3D()))
	{
		MessageBox(NULL, L"Failed to initialise Direct3D.", L"Critical Error!", MB_ICONERROR | MB_OK);
		CleanD3D();
		return;
	}
	if (FAILED(InitPipeline()))
	{
		MessageBox(NULL, L"Failed to initalize pipeline.", L"Critical Error!", MB_ICONERROR | MB_OK);
		CleanD3D();
		return;
	}



	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(CBUFFER0);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT result = _device->CreateBuffer(&cbd, NULL, &_const_buffer);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create constant buffer.");
	}
	
	cbd.ByteWidth = sizeof(PixelCBuffer);
	result = _device->CreateBuffer(&cbd, NULL, &_pixel_const_buffer);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create constant buffer.");
	}

	Instance = this;

	_cameras.push_back(new Camera());
	_cameras.push_back(new Camera());
	_camera = _cameras[_current_cam];

	AmbientLight.Type = 0;
	AmbientLight.Enabled = true;
	AmbientLight.Strength = 1;
	AmbientLight.Position = { 0,0,0, 1 };
	AmbientLight.Direction = { 0,0,0, 1 };
	AmbientLight.Colour = { 0.3f, 0.3f, 0.3f, 1 };
	AmbientLight.SpotAngle = 0;

	GameLights[0].Type = DIRECTIONAL_LIGHT;
	GameLights[0].Enabled = true;
	GameLights[0].Strength = 10;
	GameLights[0].Position = { 0.0f, 0.0f,0.0f, 0.0f };
	GameLights[0].Direction = { 0.96f, 0.4f, 0.75f, 0.3f };
	GameLights[0].Colour = { 0.4f, 0.4f, 0.0f, 1.0f };
	GameLights[0].SpotAngle = 0;

	GameLights[1].Type = POINT_LIGHT;
	GameLights[1].Enabled = true;
	GameLights[1].Strength = 10;
	GameLights[1].Position = { 10.0f, 0.0f, 10.0f, 1.0f };
	GameLights[1].Direction = { 0.0f, 0.0f, 0.0f, 1.0f };
	GameLights[1].Colour = DirectX::Colors::Red;
	GameLights[1].SpotAngle = 45;
}

Renderer::~Renderer()
{
	for (int i = 0; i < _cameras.size(); ++i)
	{
		if (!_cameras[i])
			continue;		
		delete _cameras[i];
		_cameras[i] = nullptr;
	}
	_cameras.clear();
	CleanD3D();
}

void Renderer::Draw(std::vector<GameObject*> gameObjects)
{
	_device_context->ClearRenderTargetView(_back_buffer, Colors::RosyBrown);

	if (!_camera)
		return;

	//XMFLOAT3 s;
	//XMStoreFloat3(&s, GameLights[1].Position);
	//if (Up)
	//{
	//	GameLights[1].Position = { 10.0f, s.y + 0.001f, 10.0f };
	//	if (s.y > 5)
	//		Up = false;
	//}
	//else
	//{
	//	GameLights[1].Position = { 10.0f, s.y - 0.001f, 10.0f };
	//	if (s.y < -5)
	//		Up = true;
	//}

	DrawSkybox();
	
	_device_context->ClearDepthStencilView(_depth_buffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (!gameObjects[i] || gameObjects[i] == _camera)
			continue;
		IVisualObject* visual = dynamic_cast<IVisualObject*>(gameObjects[i]);
		if (!visual)
			continue;

		//gameObjects[i]->Rotate(XMFLOAT3(0,0.001f,0));

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		_device_context->IASetVertexBuffers(0, 1, visual->GetVertexBuffer(), &stride, &offset);
		_device_context->IASetIndexBuffer(visual->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
		_device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		if (visual->Transparency)
		{
			_device_context->OMSetBlendState(_alpha_blend_enabled, 0, 0xffffffff);
			_device_context->RSSetState(_rasterizer_no_culling);
		}


		_device_context->PSSetSamplers(0, 1, visual->GetSampler());
		_device_context->PSSetShaderResources(0, 1, visual->GetTexture());


		CBUFFER0 cbuffer;
		XMMATRIX translation, rotation, scale;
		XMMATRIX world, view, projection;

		// View
		view = _camera->GetViewMatrix();

		// Projection
		projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);


		// World
		world = gameObjects[i]->GetTransform().GetWorldMatrix();
		cbuffer.worldMatrix = world;
		cbuffer.WVP = world * view * projection;

		cbuffer.inverseWorldMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, world));

		XMFLOAT3 position = gameObjects[i]->GetTransform().pos;
		cbuffer.objectWorldPos = XMLoadFloat3(&position);
		position = _camera->GetPosition();
		cbuffer.cameraPos = XMLoadFloat3(&position);


		XMMATRIX transpose = XMMatrixTranspose(world);
		PixelCBuffer pixelCBuffer;
		pixelCBuffer.AmbientLight = AmbientLight;
		for (int i = 0; i < MAX_LIGHTS; ++i)
		{
			if (!GameLights[i].Enabled)
				continue;
			pixelCBuffer.Lights[i].Type = GameLights[i].Type;
			pixelCBuffer.Lights[i].Enabled = GameLights[i].Enabled;
			pixelCBuffer.Lights[i].Strength = GameLights[i].Strength;
			pixelCBuffer.Lights[i].Position = GameLights[i].Position;

			XMVECTOR light_dir = XMVector3Transform(GameLights[i].Direction, transpose);
			pixelCBuffer.Lights[i].Direction = light_dir;
			
			pixelCBuffer.Lights[i].Colour = GameLights[i].Colour;
		}

		_device_context->UpdateSubresource(_const_buffer, 0, 0, &cbuffer, 0, 0);
		_device_context->VSSetConstantBuffers(0, 1, &_const_buffer);

		_device_context->UpdateSubresource(_pixel_const_buffer, 0, 0, &pixelCBuffer, 0, 0);
		_device_context->PSSetConstantBuffers(0, 1, &_pixel_const_buffer);
		
		_device_context->DrawIndexed(visual->GetMesh().Indices.size(), 0, 0);

		if (visual->Transparency)
		{
			_device_context->OMSetBlendState(_alpha_blend_disabled, 0, 0xffffffff);
			_device_context->RSSetState(_rasterizer_back_culling);
		}
	}
	_swapchain->Present(0, 0);
}

void Renderer::DrawSkybox()
{
	if (!_camera)
		return;
	IVisualObject* visual = dynamic_cast<IVisualObject*>(_camera->GetSkybox());
	if (!visual)
		return;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	_device_context->IASetVertexBuffers(0, 1, visual->GetVertexBuffer(), &stride, &offset);
	_device_context->IASetIndexBuffer(visual->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	_device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_device_context->RSSetState(_rasterizer_front_culling);
	_device_context->OMSetBlendState(_alpha_blend_disabled, 0, 0xffffffff);

	_device_context->PSSetSamplers(0, 1, visual->GetSampler());
	_device_context->PSSetShaderResources(0, 1, visual->GetTexture());


	CBUFFER0 cbuffer;
	XMMATRIX translation, rotation, scale;
	XMMATRIX world, view, projection;

	// View
	view = _camera->GetViewMatrix();

	// Projection
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);


	// World
	world = _camera->GetSkybox()->GetTransform().GetWorldMatrix();


	cbuffer.WVP = world * view * projection;
	//cbuffer.ambientLightCol = _skybox_ambient_light_colour;
	//cbuffer.directionalLightCol = _skybox_directional_light_colour;
	//XMMATRIX transpose = XMMatrixTranspose(world);
	//cbuffer.lightDir = XMVector3Transform(_skybox_directional_light_direction, transpose);

	_device_context->UpdateSubresource(_const_buffer, 0, 0, &cbuffer, 0, 0);
	_device_context->VSSetConstantBuffers(0, 1, &_const_buffer);
	_device_context->DrawIndexed(visual->GetMesh().Indices.size(), 0, 0);

	_device_context->RSSetState(_rasterizer_back_culling);
}

HRESULT Renderer::InitWindow(HINSTANCE instanceHandle, int nCmdShow)
{
	_instance_handle = instanceHandle;

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = Input::WindowProc;
	wc.hInstance = instanceHandle;
	wc.lpszClassName = L"WindowClass1";
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	if (!RegisterClassEx(&wc))
		return E_FAIL;

	RECT wr = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	_window_handle = CreateWindowEx(
		NULL,
		L"WindowClass1",
		_window_name,
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		100,
		100,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		instanceHandle,
		NULL);

	if (_window_handle == NULL)
		return E_FAIL;

	ShowWindow(_window_handle, nCmdShow);

	return S_OK;
}

HRESULT Renderer::InitD3D()
{
	DXGI_SWAP_CHAIN_DESC scd = {};

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = _window_handle;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;

	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&_swapchain,
		&_device,
		NULL,
		&_device_context);

	if (FAILED(hr))
		return hr;

	ID3D11Texture2D* pBackBufferTexture = nullptr;
	hr = _swapchain->GetBuffer(0, _uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);

	hr = _device->CreateRenderTargetView(pBackBufferTexture, NULL, &_back_buffer);

	pBackBufferTexture->Release();

	if (FAILED(hr))
		return hr;


	D3D11_TEXTURE2D_DESC text2DDesc = { 0 };
	text2DDesc.Width = SCREEN_WIDTH;
	text2DDesc.Height = SCREEN_HEIGHT;
	text2DDesc.ArraySize = 1;
	text2DDesc.MipLevels = 1;
	text2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	text2DDesc.SampleDesc.Count = scd.SampleDesc.Count;
	text2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	text2DDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* zBufferTexture;
	hr = _device->CreateTexture2D(&text2DDesc, NULL, &zBufferTexture);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create Z Buffer Texture.");
		return E_FAIL;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = text2DDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = _device->CreateDepthStencilView(zBufferTexture, &dsvDesc, &_depth_buffer);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create depth stencil view.");
		return E_FAIL;
	}
	zBufferTexture->Release();

	_device_context->OMSetRenderTargets(1, &_back_buffer, _depth_buffer);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	_device_context->RSSetViewports(1, &viewport);


	D3D11_RASTERIZER_DESC rast_none_desc;
	ZeroMemory(&rast_none_desc, sizeof(D3D11_RASTERIZER_DESC));
	rast_none_desc.CullMode = D3D11_CULL_NONE;
	rast_none_desc.FillMode = D3D11_FILL_SOLID;
	_device->CreateRasterizerState(&rast_none_desc, &_rasterizer_no_culling);

	D3D11_RASTERIZER_DESC rast_back_desc;
	ZeroMemory(&rast_back_desc, sizeof(D3D11_RASTERIZER_DESC));
	rast_back_desc.CullMode = D3D11_CULL_BACK;
	rast_back_desc.FillMode = D3D11_FILL_SOLID;
	_device->CreateRasterizerState(&rast_back_desc, &_rasterizer_back_culling);

	D3D11_RASTERIZER_DESC rast_front_desc;
	ZeroMemory(&rast_front_desc, sizeof(D3D11_RASTERIZER_DESC));
	rast_front_desc.CullMode = D3D11_CULL_FRONT;
	rast_front_desc.FillMode = D3D11_FILL_SOLID;
	_device->CreateRasterizerState(&rast_front_desc, &_rasterizer_front_culling);

	_device_context->RSSetState(_rasterizer_back_culling);

	D3D11_BLEND_DESC bd1 = { 0 };
	bd1.RenderTarget[0].BlendEnable = TRUE;
	bd1.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd1.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd1.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd1.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd1.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd1.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd1.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd1.IndependentBlendEnable = FALSE;
	bd1.AlphaToCoverageEnable = TRUE;
	_device->CreateBlendState(&bd1, &_alpha_blend_enabled);

	D3D11_BLEND_DESC bd2 = { 0 };
	bd2.RenderTarget[0].BlendEnable = FALSE;
	bd2.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd2.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd2.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd2.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd2.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd2.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd2.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd2.IndependentBlendEnable = FALSE;
	bd2.AlphaToCoverageEnable = FALSE;
	_device->CreateBlendState(&bd2, &_alpha_blend_disabled);
	return S_OK;
}

HRESULT Renderer::InitPipeline()
{
	HRESULT result;
	auto vertexShaderBytecode = DX::ReadData(L"VertexShader.cso");
	auto pixelShaderBytecode = DX::ReadData(L"PixelShader.cso");

	result = _device->CreateVertexShader(vertexShaderBytecode.data(), vertexShaderBytecode.size(), NULL, &_vertex_shader);
	if (FAILED(result))
	{
		return result;
	}

	result = _device->CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), NULL, &_pixel_shader);
	if (FAILED(result))
	{
		return result;
	}

	_device_context->VSSetShader(_vertex_shader, 0, 0);
	_device_context->PSSetShader(_pixel_shader, 0, 0);

	ID3D11ShaderReflection* vShaderReflection = NULL;
	result = D3DReflect(vertexShaderBytecode.data(), vertexShaderBytecode.size(),
		IID_ID3D11ShaderReflection, (void**)&vShaderReflection);
	if (FAILED(result))
	{
		return result;
	}

	D3D11_SHADER_DESC desc;
	result = vShaderReflection->GetDesc(&desc);
	if (FAILED(result))
	{
		return result;
	}

	D3D11_SIGNATURE_PARAMETER_DESC* signatureParamDescriptions =
		new D3D11_SIGNATURE_PARAMETER_DESC[desc.InputParameters]{ 0 };
	for (UINT i = 0; i < desc.InputParameters; ++i)
	{
		vShaderReflection->GetInputParameterDesc(i, &signatureParamDescriptions[i]);
	}

	D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[desc.InputParameters]{ 0 };
	for (size_t i = 0; i < desc.InputParameters; ++i)
	{
		ied[i].SemanticName = signatureParamDescriptions[i].SemanticName;
		ied[i].SemanticIndex = signatureParamDescriptions[i].SemanticIndex;
		if (signatureParamDescriptions[i].ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
		{
			switch (signatureParamDescriptions[i].Mask)
			{
			case 1: ied[i].Format = DXGI_FORMAT_R32_FLOAT;  break;
			case 3:	ied[i].Format = DXGI_FORMAT_R32G32_FLOAT;  break;
			case 7:	ied[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;  break;
			case 15:ied[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;  break;
			default: break;
			}
		}
		ied[i].InputSlot = 0;
		ied[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		ied[i].InstanceDataStepRate = 0;
	}

	result = _device->CreateInputLayout(ied, desc.InputParameters,
		vertexShaderBytecode.data(), vertexShaderBytecode.size(), &_layout);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create input layout...");
		return result;
	}

	_device_context->IASetInputLayout(_layout);

	delete[] signatureParamDescriptions;
	delete[] ied;
	return S_OK;
}

void Renderer::CleanD3D()
{
	if (_vertex_shader)
		_vertex_shader->Release();
	if (_pixel_shader)
		_pixel_shader->Release();
	if (_layout)
		_layout->Release();

	if (_rasterizer_no_culling)
		_rasterizer_no_culling->Release();
	if (_rasterizer_back_culling)
		_rasterizer_back_culling->Release();
	if (_rasterizer_front_culling)
		_rasterizer_front_culling->Release();

	if (_alpha_blend_enabled)
		_alpha_blend_enabled->Release();
	if (_alpha_blend_disabled)
		_alpha_blend_disabled->Release();

	if (_depth_buffer)
		_depth_buffer->Release();
	if (_back_buffer)
		_back_buffer->Release();

	if (_swapchain)
		_swapchain->Release();
	if (_device)
		_device->Release();
	if (_device_context)
		_device_context->Release();
}

void Renderer::SwitchCamera()
{
	_current_cam++;
	if (_current_cam >= _cameras.size())
		_current_cam = 0;
	if (_cameras[_current_cam])
		_camera = _cameras[_current_cam];
}
