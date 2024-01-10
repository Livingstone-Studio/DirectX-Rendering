#include "Renderer.h"

#include "../Core/Time.h"
#include "../Core/Input.h"
#include "../Rendering/External/ReadData.h"
#include "../GameObjects/GameObject.h"
#include "../GameObjects/TextObject.h"
#include "../GameObjects/Camera.h"
#include "Custom OBJ Loader/ObjModelLoader.h"
#include "Custom OBJ Loader/Model.h"
#include <chrono>
#include <iostream>

Renderer* Renderer::Instance{ nullptr };

Renderer::Renderer(GameWindow* window)
{
	m_window = window;

	if (!m_window)
	{
		MessageBox(NULL, L"Failed to find a window.", L"Critical Error!", MB_ICONERROR | MB_OK);
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
	HRESULT result = m_device->CreateBuffer(&cbd, NULL, &m_buffers[DEFAULT_VERTEX_BUFFER]);
	if (FAILED(result))
		OutputDebugString(L"Failed to create constant buffer.");
	cbd.ByteWidth = sizeof(PixelCBuffer);
	result = m_device->CreateBuffer(&cbd, NULL, &m_buffers[DEFAULT_PIXEL_BUFFER]);
	if (FAILED(result))
		OutputDebugString(L"Failed to create constant buffer.");
	cbd.ByteWidth = sizeof(SKYBOXCBUFFER0);
	result = m_device->CreateBuffer(&cbd, NULL, &m_buffers[SKYBOX_CONSTANT_BUFFER]); // HREE
	if (FAILED(result))
		OutputDebugString(L"Failed to create constant buffer.");

	Instance = this;

	_cameras.push_back(new Camera(false));
	_cameras.push_back(new Camera(true));
	_camera = _cameras[_current_cam];

	m_screen_font = std::make_unique<SpriteFont>(m_device, L"Assets/wildwest.spritefont");

	m_font_batch = std::make_unique<SpriteBatch>(m_device_context);

	AmbientLight.Type = 0;
	AmbientLight.Enabled = true;
	AmbientLight.Strength = 1;
	AmbientLight.Position = { 0,0,0, 1 };
	AmbientLight.Direction = { 0,0,0, 1 };
	AmbientLight.Colour = { 0.02f, 0.02f, 0.02f, 1 };
	AmbientLight.SpotAngle = 0;

	GameLights[0].Type = DIRECTIONAL_LIGHT;
	GameLights[0].Enabled = true;
	GameLights[0].Strength = 1;
	GameLights[0].Position = { 0.0f, 0.0f,0.0f, 0.0f };
	GameLights[0].Direction = { 0.96f, 0.4f, 0.75f, 0.3f };
	GameLights[0].Colour = { 0.1f, 0.1f, 0.2f, 1.0f };
	GameLights[0].SpotAngle = 0;
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
	m_device_context->ClearRenderTargetView(m_back_buffer, Colors::RosyBrown);

	if (!_camera)
		return;

	DrawSkybox();

	m_device_context->ClearDepthStencilView(m_depth_buffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	Model* model;
	for (int i = 0; i < gameObjects.size(); ++i)
	{
		if (!gameObjects[i])
			continue;
		model = gameObjects[i]->GetModel();
		if (model)
			model->Draw(this);
	}

	RenderTextObject();

	//if (m_screen_font)
	//{
	//	// Get normalized pos value for target res
	//	float norm_pos = 1.0f;

	//	// Find current res
	//	float width = m_window->GetScreenSize()[0];
	//	float height = m_window->GetScreenSize()[1];

	//	// Find nomralized change in new res from target res
	//	width = width / 800;
	//	height = height / 600;

	//	// multiply with original target pos
	//	int pos_weight_w = norm_pos / width;
	//	int pos_weight_h  = norm_pos / height;

	//	m_font_position.x = (m_window->GetScreenSize()[0]) * pos_weight_w;
	//	m_font_position.y = (m_window->GetScreenSize()[1]) * pos_weight_h;

	//	float sca = 10.0f;
	//	float scaMult = sca * 0.0001f;

	//	SimpleMath::Vector2 scale;
	//	scale.x = (800) * (scaMult / width);
	//	scale.y = (600) * (scaMult / height);


	//	m_font_batch->Begin();

	//	std::wstring counter = L"FPS: " + to_wstring(Time::GetFPS());
	//	const wchar_t* output = counter.c_str();

	//	SimpleMath::Vector2 origin = { 0,0 };

	//	m_screen_font->DrawString(m_font_batch.get(), output, m_font_position, Colors::White, 0.0f, origin, scale);

	//	m_font_batch->End();
	//}

	m_swapchain->Present(0, 0);
}

void Renderer::DrawSkybox()
{
	if (!_camera)
		return;

	m_device_context->OMSetDepthStencilState(GetDepthStencilState(NONE_DEPTH_STATE), 1);
	m_device_context->RSSetState(GetRasterizerState(RAST_FRONT_CULL));

	m_device_context->VSSetShader(GetVertexShader(SKYBOX_VERTEX_SHADER), 0, 0);
	m_device_context->PSSetShader(GetPixelShader(SKYBOX_PIXEL_SHADER), 0, 0);
	m_device_context->IASetInputLayout(GetInputLayout(SKYBOX_INPUT_LAYOUT));
	m_device_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	SKYBOXCBUFFER0 sbcbuff;

	XMMATRIX translation, projection, view;
	translation = XMMatrixTranslation(_camera->GetPosition().x, _camera->GetPosition().y, _camera->GetPosition().z);
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), m_window->GetScreenSize()[0] / m_window->GetScreenSize()[1], 0.1f, 100.0f);
	view = _camera->GetViewMatrix();
	sbcbuff.WVP = translation * view * projection;
	m_device_context->UpdateSubresource(m_buffers[SKYBOX_CONSTANT_BUFFER], 0, 0, &sbcbuff, 0, 0);

	m_device_context->VSSetConstantBuffers(0, 1, &m_buffers[SKYBOX_CONSTANT_BUFFER]);
	m_device_context->PSSetSamplers(0, 1, &m_skybox_sampler);
	m_device_context->PSSetShaderResources(0, 1, &m_skybox_texture);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_device_context->IASetVertexBuffers(0, 1, &m_skybox_vertex_buffer, &stride, &offset);
	m_device_context->Draw(m_skybox.vertices.size(), 0);

	m_device_context->OMSetDepthStencilState(GetDepthStencilState(SOLID_DEPTH_STATE), 1);
	m_device_context->RSSetState(GetRasterizerState(RAST_BACK_CULL));


	m_device_context->VSSetShader(GetVertexShader(DEFAULT_VERTEX_SHADER), 0, 0);
	m_device_context->PSSetShader(GetPixelShader(DEFAULT_PIXEL_SHADER), 0, 0);
	m_device_context->IASetInputLayout(GetInputLayout(DEFAULT_INPUT_LAYOUT));
}

const int& Renderer::GetFreeLight()
{
	for (int i = 0; i < MAX_LIGHTS; ++i)
		if (!GameLights[i].Enabled)
			return i;
	return -1;
}

HRESULT Renderer::InitD3D()
{
	DXGI_SWAP_CHAIN_DESC scd = {};

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = (UINT)m_window->GetScreenSize()[0];
	scd.BufferDesc.Height = (UINT)m_window->GetScreenSize()[1];
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = m_window->GetWindowHandle();
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
		&m_swapchain,
		&m_device,
		NULL,
		&m_device_context);

	if (FAILED(hr))
		return hr;

	ID3D11Texture2D* pBackBufferTexture = nullptr;
	hr = m_swapchain->GetBuffer(0, _uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);

	hr = m_device->CreateRenderTargetView(pBackBufferTexture, NULL, &m_back_buffer);

	pBackBufferTexture->Release();

	if (FAILED(hr))
		return hr;


	D3D11_TEXTURE2D_DESC text2DDesc = { 0 };
	text2DDesc.Width = (UINT)m_window->GetScreenSize()[0];
	text2DDesc.Height = (UINT)m_window->GetScreenSize()[1];
	text2DDesc.ArraySize = 1;
	text2DDesc.MipLevels = 1;
	text2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	text2DDesc.SampleDesc.Count = scd.SampleDesc.Count;
	text2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	text2DDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* zBufferTexture;
	hr = m_device->CreateTexture2D(&text2DDesc, NULL, &zBufferTexture);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create Z Buffer Texture.");
		return E_FAIL;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_device->CreateSamplerState(&samplerDesc, &m_skybox_sampler);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = text2DDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = m_device->CreateDepthStencilView(zBufferTexture, &dsvDesc, &m_depth_buffer);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create depth stencil view.");
		return E_FAIL;
	}
	zBufferTexture->Release();

	m_device_context->OMSetRenderTargets(1, &m_back_buffer, m_depth_buffer);

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (UINT)m_window->GetScreenSize()[0];
	viewport.Height = (UINT)m_window->GetScreenSize()[1];
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	m_device_context->RSSetViewports(1, &viewport);

	// Rasterizer States Creation.
	D3D11_RASTERIZER_DESC rast_desc;
	ZeroMemory(&rast_desc, sizeof(D3D11_RASTERIZER_DESC));
	rast_desc.FillMode = D3D11_FILL_SOLID;
	rast_desc.CullMode = D3D11_CULL_NONE;
	m_device->CreateRasterizerState(&rast_desc, &m_rast_states[RAST_NO_CULL]);
	rast_desc.CullMode = D3D11_CULL_BACK;
	m_device->CreateRasterizerState(&rast_desc, &m_rast_states[RAST_BACK_CULL]);
	rast_desc.CullMode = D3D11_CULL_FRONT;
	m_device->CreateRasterizerState(&rast_desc, &m_rast_states[RAST_FRONT_CULL]);
	m_device_context->RSSetState(GetRasterizerState(RAST_BACK_CULL));

	// Depth Stencil States Creation.
	D3D11_DEPTH_STENCIL_DESC depth_desc = { 0 };
	depth_desc.DepthEnable = true;
	depth_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_desc.DepthFunc = D3D11_COMPARISON_LESS;
	m_device->CreateDepthStencilState(&depth_desc, &m_depth_states[SOLID_DEPTH_STATE]);
	depth_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_device->CreateDepthStencilState(&depth_desc, &m_depth_states[NONE_DEPTH_STATE]);

	// Blend States Creation.
	D3D11_BLEND_DESC blend_desc = { 0 };
	blend_desc.RenderTarget[0].BlendEnable = TRUE;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.AlphaToCoverageEnable = TRUE;
	m_device->CreateBlendState(&blend_desc, &m_blend_states[BLEND_ENABLED]);
	blend_desc.RenderTarget[0].BlendEnable = FALSE;
	blend_desc.AlphaToCoverageEnable = FALSE;
	m_device->CreateBlendState(&blend_desc, &m_blend_states[BLEND_DISABLED]);

	Model* temp = nullptr;
	HRESULT result = ObjModelLoader::LoadModel("Assets/cube.obj", "", &temp);
	if (FAILED(result))
		OutputDebugString(L"Failed to load skybox.");
	if (temp)
	{
		m_skybox = temp->GetModelData();
	}

	D3D11_BUFFER_DESC bd = { 0 };
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * m_skybox.vertices.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = m_device->CreateBuffer(&bd, NULL, &m_skybox_vertex_buffer);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create vertex buffer.");
		return E_FAIL;
	}

	D3D11_MAPPED_SUBRESOURCE ms;
	m_device_context->Map(m_skybox_vertex_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, m_skybox.vertices.data(), sizeof(Vertex) * m_skybox.vertices.size());
	m_device_context->Unmap(m_skybox_vertex_buffer, NULL);

	CreateDDSTextureFromFile(m_device, m_device_context, L"Assets/skybox01.dds", NULL, &m_skybox_texture);

	return S_OK;
}

HRESULT Renderer::InitPipeline()
{
	LoadVertexShader(L"VertexShader.cso", &m_vertex_shaders[DEFAULT_VERTEX_SHADER], &m_layouts[DEFAULT_INPUT_LAYOUT]);
	LoadPixelShader(L"PixelShader.cso", &m_pixel_shaders[DEFAULT_PIXEL_SHADER]);

	LoadVertexShader(L"SkyboxVertexShader.cso", &m_vertex_shaders[SKYBOX_VERTEX_SHADER], &m_layouts[SKYBOX_INPUT_LAYOUT]);
	LoadPixelShader(L"SkyboxPixelShader.cso", &m_pixel_shaders[SKYBOX_PIXEL_SHADER]);


	return S_OK;
}

void Renderer::CleanD3D()
{
	if (m_screen_font)
		m_screen_font.reset();

	if (m_font_batch)
		m_font_batch.reset();

	for (int i = 0; i < VERTEX_SHADER_COUNT; ++i)
		if (m_vertex_shaders[i])
			m_vertex_shaders[i]->Release();
	for (int i = 0; i < PIXEL_SHADER_COUNT; ++i)
		if (m_pixel_shaders[i])
			m_pixel_shaders[i]->Release();
	for (int i = 0; i < BUFFER_COUNT; ++i)
		if (m_buffers[i])
			m_buffers[i]->Release();
	for (int i = 0; i < RAST_STATE_COUNT; ++i)
		if (m_rast_states[i])
			m_rast_states[i]->Release();
	for (int i = 0; i < BLEND_STATE_COUNT; ++i)
		if (m_blend_states[i])
			m_blend_states[i]->Release();
	for (int i = 0; i < INPUT_LAYOUT_COUNT; ++i)
		if (m_layouts[i])
			m_layouts[i]->Release();
	for (int i = 0; i < DEPTH_STATE_COUNT; ++i)
		if (m_depth_states[i])
			m_depth_states[i]->Release();

	if (m_skybox_sampler)
		m_skybox_sampler->Release();
	if (m_skybox_texture)
		m_skybox_texture->Release();
	if (m_skybox_vertex_buffer)
		m_skybox_vertex_buffer->Release();
	
	if (m_depth_buffer)
		m_depth_buffer->Release();
	if (m_back_buffer)
		m_back_buffer->Release();

	if (m_swapchain)
		m_swapchain->Release();

	if (m_device)
		m_device->Release();
	if (m_device_context)
		m_device_context->Release();
}

void Renderer::RenderTextObject()
{
	if (!m_screen_font || m_text.size() == 0)
		return;
	for (int i = 0; i < m_text.size(); ++i)
	{
		if (!m_text[i])
			continue;
		RenderText(m_text[i]);
	}
}

void Renderer::RenderText(TextObject* text)
{
	// Find norm res
	float width = m_window->GetScreenSize()[0] / m_text_target_resolution[0];
	float height = m_window->GetScreenSize()[1] / m_text_target_resolution[1];

	// multiply with original target pos
	float pos_weight_w = text->GetScreenPosition().x / width;
	float pos_weight_h = text->GetScreenPosition().y / height;

	m_font_position.x = (m_window->GetScreenSize()[0]) * pos_weight_w;
	m_font_position.y = (m_window->GetScreenSize()[1]) * pos_weight_h;

	float scaMult = text->GetScale() * m_text_scale;

	SimpleMath::Vector2 scale;
	scale.x = (m_text_target_resolution[0]) * (scaMult / width);
	scale.y = (m_text_target_resolution[1]) * (scaMult / height);

	const wchar_t* output = text->GetText().c_str();

	m_font_batch->Begin();
	m_screen_font->DrawString(m_font_batch.get(), output, m_font_position, Colors::White, 0.0f, { 0,0 }, scale);
	m_font_batch->End();
}

void Renderer::SwitchCamera()
{
	_current_cam++;
	if (_current_cam >= _cameras.size())
		_current_cam = 0;
	if (_cameras[_current_cam])
		_camera = _cameras[_current_cam];
}

HRESULT Renderer::LoadVertexShader(LPCWSTR filename, ID3D11VertexShader** vs, ID3D11InputLayout** il)
{
	HRESULT result;
	auto vertexShaderBytecode = DX::ReadData(filename);

	result = m_device->CreateVertexShader(vertexShaderBytecode.data(), vertexShaderBytecode.size(), NULL, vs);
	if (FAILED(result))
	{
		return result;
	}

	ID3D11ShaderReflection* vShaderReflection = NULL;
	result = D3DReflect(vertexShaderBytecode.data(), vertexShaderBytecode.size(), IID_ID3D11ShaderReflection, (void**)&vShaderReflection);
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
	D3D11_SIGNATURE_PARAMETER_DESC* signatureParamDescriptions = new D3D11_SIGNATURE_PARAMETER_DESC[desc.InputParameters]{ 0 };
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
	result = m_device->CreateInputLayout(ied, desc.InputParameters, vertexShaderBytecode.data(), vertexShaderBytecode.size(), il);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create input layout...");
		return result;
	}

	delete[] signatureParamDescriptions;
	delete[] ied;
	return S_OK;
}

HRESULT Renderer::LoadPixelShader(LPCWSTR filename, ID3D11PixelShader** ps)
{
	HRESULT result;
	auto pixelShaderBytecode = DX::ReadData(filename);

	result = m_device->CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), NULL, ps);
	if (FAILED(result))
	{
		return result;
	}
	return S_OK;
}