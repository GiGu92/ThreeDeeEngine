#pragma once

#include <windows.h>

#include "Common.h"
#include "Camera.h"
#include "Drawable.h"

class Engine
{
public:
	
	Engine();
	~Engine();

	void Init(HWND hWnd, float viewportWidth, float viewportHeight);
	void Release();

	void InitD3D(HWND hWnd);
	void ReleaseD3D();
	void InitViewport(float w, float h);
	void InitPipeline();
	void ReleasePipeline();
	void InitScene();
	void ReleaseScene();

	void Update(float elapsedTime);
	void RenderFrame();

	float GetTime() { return time; }
	float GetDeltaTime() { return deltaTime; }

	struct PerFrameConstantBufferData { XMMATRIX view; XMMATRIX projection; };
	struct PerObjectConstantBufferData { XMMATRIX world; };

	struct CameraInputState
	{
		bool isMovingForward;
		bool isMovingBackward;
		bool isMovingRight;
		bool isMovingLeft;
		bool isMovingUp;
		bool isMovingDown;
		bool isSpeeding;
		int deltaYaw;
		int deltaPitch;
	};
	CameraInputState cameraInputState;

private:

	float time, deltaTime;

	Camera *camera;
	float cameraMoveSpeed, cameraTurnSpeed;

	ID3D11Device *device;
	ID3D11DeviceContext *context;
	IDXGISwapChain *swapchain;
	ID3D11RenderTargetView *backbuffer;

	ID3D11Buffer *perFrameCB;
	ID3D11Buffer *perObjectCB;

	ID3D11InputLayout *standardInputLayout;
	ID3D11VertexShader *standardVS;
	ID3D11PixelShader *standardOpaquePS;

	Drawable *triangle;
	Drawable *box;
};

extern Engine *gEngine;