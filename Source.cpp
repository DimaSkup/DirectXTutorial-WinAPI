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

// function prototypes
void InitD3D(HWND hWnd);
void CleanD3D(void);

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
			// run here your game code
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
	}

	DefWindowProc(hWnd, message, wParam, lParam);
}

void InitD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 4;
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
}



void CleanD3D(void)
{
	swapchain->Release();
	dev->Release();
	devcon->Release();
}