#include <windows.h>
#include <windowsx.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// global variables
IDXGISwapChain *swapchain = nullptr;
ID3D11Device *dev = nullptr;
ID3D11DeviceContext *devcon = nullptr;

ID3D11RenderTargetView *backbuffer = nullptr;

// functions prototypes
void InitD3D(HWND hWnd);
void RenderFrame(void);
void ClearD3D(void);

// our custom handler of events from windows
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

	ZeroMemory(&wc, sizeof(wc));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wc.lpszClassName = L"WindowClass1";

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, 500, 400 };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	hWnd = CreateWindowEx(NULL,
						L"WindowClass1",
						L"Our first window program!",
						WS_OVERLAPPEDWINDOW,
						300, 
						300,
						wr.right - wr.left,
						wr.bottom - wr.top,
						NULL,
						NULL,
						hInstance,
						NULL);

	ShowWindow(hWnd, nCmdShow);

	// initialize directX stuff
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
			// do here your game stuff
			RenderFrame();
		}
	}

	// clear up COM objects, etc.
	ClearD3D();

	return msg.wParam;
}


LPARAM CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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




// directX initialization
void InitD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 4;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;

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


	// initialize the render target view
	ID3D11Texture2D *pBackBuffer = nullptr;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	devcon->OMSetRenderTargets(1, &backbuffer, NULL);


	// initialize the viewport
	D3D11_VIEWPORT viewport;

	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 800;
	viewport.Height = 600;

	devcon->RSSetViewports(1, &viewport);
}

FLOAT colorVal = 1.0f;
FLOAT colorStep = 0.01f;
int tickCount = 0;

void RenderFrame(void)
{
	// each 0.1 second we change the color of the render target view
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

void ClearD3D(void)
{
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}