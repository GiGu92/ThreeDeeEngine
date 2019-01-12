#include "Engine.h"

Engine *gEngine;

Engine::Engine()
{
}

Engine::~Engine()
{
}

void Engine::Init(HWND hWnd, float viewportWidth, float viewportHeight)
{
	InitD3D(hWnd);
	InitViewport(viewportWidth, viewportHeight);
	InitPipeline();
	InitGraphics();
}

void Engine::Release()
{
	SAFE_RELEASE(standardInputLayout);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(standardVS);
	SAFE_RELEASE(standardOpaquePS);

	ReleaseD3D();
}

void Engine::InitD3D(HWND hWnd)
{
	// Direct3D initialization

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.Windowed = true;

	D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
		&scd, &swapchain, &device, nullptr, &context);

	// Set the render target

	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	device->CreateRenderTargetView(pBackBuffer, nullptr, &backbuffer);
	pBackBuffer->Release();

	context->OMSetRenderTargets(1, &backbuffer, nullptr);
}

void Engine::InitViewport(float w, float h)
{
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = w;
	viewport.Height = h;

	context->RSSetViewports(1, &viewport);
}

void Engine::InitPipeline()
{
	ID3DBlob *vsBlob, *psBlob, *errorBlob;

	HRESULT hr;

	hr = D3DCompileFromFile(L"Shaders\\Standard.shader", nullptr, nullptr, "StandardVS", "vs_4_0", 0, 0, &vsBlob, &errorBlob);
	if (errorBlob)
	{
		OutputDebugString(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
		errorBlob = nullptr;
	}
	if (FAILED(hr))
		throw;

	hr = D3DCompileFromFile(L"Shaders\\Standard.shader", nullptr, nullptr, "StandardOpaquePS", "ps_4_0", 0, 0, &psBlob, &errorBlob);
	if (errorBlob)
	{
		OutputDebugString(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
		errorBlob = nullptr;
	}
	if (FAILED(hr))
		throw;

	device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &standardVS);
	device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &standardOpaquePS);

	context->VSSetShader(standardVS, nullptr, 0);
	context->PSSetShader(standardOpaquePS, nullptr, 0);

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	device->CreateInputLayout(ied, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &standardInputLayout);

	context->IASetInputLayout(standardInputLayout);
}

void Engine::InitGraphics()
{
	StandardVertexData vertices[] =
	{
		{  0.0f,    0.5f, 0.0f, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{  0.45f,  -0.5,  0.0f, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{  -0.45f, -0.5f, 0.0f, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC; // write access access by CPU and GPU
	bd.ByteWidth = sizeof(StandardVertexData) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	

	device->CreateBuffer(&bd, nullptr, &vertexBuffer);

	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	memcpy(ms.pData, vertices, sizeof(vertices));
	context->Unmap(vertexBuffer, 0);
}

void Engine::ReleaseD3D()
{
	SAFE_RELEASE(swapchain);
	SAFE_RELEASE(backbuffer);
	SAFE_RELEASE(device);
	SAFE_RELEASE(context);
}

void Engine::RenderFrame(void)
{
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	context->ClearRenderTargetView(backbuffer, clearColor);

	UINT stride = sizeof(StandardVertexData);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->Draw(3, 0);

	swapchain->Present(0, 0);
}