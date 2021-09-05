#include <windows.h>
#include <windowsx.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")


// define the screen resolution
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600


// global variables, COM objects, etc.
IDXGISwapChain *swapchain = nullptr;
ID3D11Device *dev = nullptr;
ID3D11DeviceContext *devcon = nullptr;

ID3D11RenderTargetView *backbuffer = nullptr;

ID3D11VertexShader *pVS;		// the vertex shader
ID3D11PixelShader  *pPS;		// the pixel shader


struct VERTEX					// a struct to define a vertex
{
	FLOAT X, Y, Z;
	D3DXCOLOR Color;
};

ID3D11Buffer *pVBuffer = nullptr;	// the vertex buffer



void InitPipeline(void);

void InitD3D(HWND hWnd);
void CleanD3D(void);
void RenderFrame(void);

LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wc.lpszClassName = L"WindowClass1";

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	hWnd = CreateWindowEx(NULL,
		L"WindowClass1",
		L"Our first window program!",
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);

	InitD3D(hWnd);

	// main loop

	MSG msg;

	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
		else
		{
			// do here game stuff
			RenderFrame();
		}
	}

	CleanD3D();

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	}

	DefWindowProc(hWnd, message, wParam, lParam);
}

void InitD3D(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;								// one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// use 32-bit color
	scd.BufferDesc.Width = SCREEN_WIDTH;				// set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT;				// set the back buffer height 
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// how swap chain is to be used
	scd.OutputWindow = hWnd;							// the window to be used
	scd.SampleDesc.Count = 4;							// how many multisamples
	scd.SampleDesc.Quality = 0;							// multisample quality level
	scd.Windowed = TRUE;								// windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// allow full-screen switching

														// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon);

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer = nullptr;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	// set the render targe as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);


	// set the viewport
	D3D11_VIEWPORT viewport;

	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	devcon->RSSetViewports(1, &viewport);
}


FLOAT colorVal = 1.0f;
FLOAT colorStep = 0.01f;
int tickCount = 0;

void RenderFrame(void)
{
	// each 0.1 second we change the color of the render target view
	// from deep blue to black
	if ((GetTickCount() / 100) != tickCount)
	{
		tickCount = GetTickCount() / 100;
		colorVal -= colorStep;

		devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(colorVal - 1.0f, colorVal - 0.8f, colorVal - 0.6f, 1.0f));
	}


	// here you can render into your backbuffer
	// ...

	swapchain->Present(0, 0);
}



// this is the function that cleans up Direct3D and COM
void CleanD3D()
{
	swapchain->SetFullscreenState(FALSE, NULL);

	// close and release all existing COM objects
	pVS->Release();
	pPS->Release();
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}



void InitPipeline(void)
{
	// load and compile the two shaders
	ID3D10Blob *VS;		// we'll put here a compiled code of the vertex shader
	ID3D10Blob *PS;		// we'll put here a compiled code of the pixel shader

						// compile the vertex shader
	D3DX11CompileFromFile(L"shaders.shader",
		NULL,
		NULL,
		"VShader",
		"vs_4_0",
		NULL,
		NULL,
		NULL,
		&VS,
		NULL,
		NULL);

	// compile the pixel shader 
	D3DX11CompileFromFile(L"shaders.shader",
		NULL,
		NULL,
		"PShader",
		"ps_4_0",
		NULL,
		NULL,
		NULL,
		&PS,
		NULL,
		NULL);

	// encapsulate both shaders into shader objects
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	// set the shader objects
	devcon->VSSetShader(pVS, NULL, NULL);
	devcon->PSSetShader(pPS, NULL, NULL);
}


void InitGraphics(void)
{
	// create a triangle using the VERTEX struct
	VERTEX OurVertices[] = {
		{ 0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ -0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
	};

	// create the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.Usage = D3D11_USAGE_DYNAMIC;				// write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 3;			// size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// allow CPU to write in the buffer

	dev->CreateBuffer(&bd, NULL, &pVBuffer);	// create the buffer




	D3D11_MAPPED_SUBRESOURCE ms;

	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));
	devcon->Unmap(pVBuffer, NULL);
}