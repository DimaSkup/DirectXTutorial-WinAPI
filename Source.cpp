// include the basic windows header file
#include <windows.h>
#include <windowsx.h>

// Direct3D headers
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// global declarations
IDXGISwapChain *swapchain;		// the pointer to the swap chain interface
ID3D11Device *dev;				// the pointer to the our Direct3D device interface
ID3D11DeviceContext *devcon;	// the pointer to the our Direct3D device context
ID3D11RenderTargetView *backbuffer;	// the pointer to the our back buffer


// function prototypes
void InitD3D(HWND hWnd);		// sets up and initializes Direct3D
void CleanD3D(void);			// closes Direct3D and releases memory
void RenderFrame(void);			// used to render a single frame


// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
							UINT message,
							WPARAM wParam,
							LPARAM lParam);

// the entry point of any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nCmdShow)
{
	// the handle for the window, filled by a function
	HWND hWnd;

	// this struct holds information for the window class
	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"WindowClass1";

	// register the window class
	RegisterClassEx(&wc);

	RECT wr = { 0, 0, 500, 400 };	// set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);	// adjust the size
	
	// create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
						  L"WindowClass1",	// name of the window class
						  L"Our First Windowed Program",	// title of the window
						  WS_OVERLAPPEDWINDOW,	// window style
						  300,		// x-position of the window
						  300,		// y-position of the window
						  SCREEN_WIDTH,		// width of the window
						  SCREEN_HEIGHT,	// height of the window
						  NULL,		// we have no parent window, NULL
						  NULL,		// we aren't using menus, NULL
						  hInstance,	// application handle
						  NULL);	// used with multiple windows, NULL
	
	// display the window on the screen
	ShowWindow(hWnd, nCmdShow);

	// enter the main loop:

	// this struct holds Windows event messages
	MSG msg = {0};

	
	// Enter the infinite message loop
	while (TRUE)
	{
		// Check to see if any messages are waiting in the queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// translate keystroke messages into the right format
			TranslateMessage(&msg);

			// send the message to the WindowProc function
			DispatchMessage(&msg);

			// check to see if it's time to quit
			if (msg.message == WM_QUIT)
				break;
		}
		else
		{
			// Run game code here
			InitD3D(hWnd);
			RenderFrame();
			CleanD3D();
		}
		
	}

	
	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
		case WM_DESTROY:
		{
			// close the application entirely
			PostQuitMessage(0);
			return 0;
		} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}



/////////////////////////////////
//
// LAUNCHING DIRECT3D
//
/////////////////////////////////

// this function intialize and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;									// one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// use 32-bit colour
	scd.BufferDesc.Width = SCREEN_WIDTH;					// set window width resolution
	scd.BufferDesc.Height = SCREEN_HEIGHT;					// set window height resolution
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// how swap chain is to be used
	scd.OutputWindow = hWnd;								// the window to be used
	scd.SampleDesc.Count = 4;								// how many multisamples
	scd.Windowed = TRUE;									// windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		// allow full-screen switching

	// create a device, device context and swap chain using the information
	// in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,						// *pAdapter
								  D3D_DRIVER_TYPE_HARDWARE,	// DriverType
								  NULL,						// SoftWare
								  NULL,						// Flags
								  NULL,						// *pFeatureLevels
								  NULL,						// FeatureLevels
								  D3D11_SDK_VERSION,		// SDKVersion
								  &scd,						// *pSwapChainDesc
								  &swapchain,				// **ppSwapChain
								  &dev,						// **ppDevice
								  NULL,						// *pFeatureLevel
								  &devcon);					// **ppDeviceContext

	

	// Establishing of a render target (back buffer)
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// Use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	// Set the render target as the back buffer
	devcon->OMSetRenderTargets(1, &backbuffer, NULL);



	//--- SET THE VIEW PORT ---//
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	devcon->RSSetViewports(1, &viewport);

}

// this is the function that cleans up Direct3D and COM
void CleanD3D()
{
	swapchain->SetFullscreenState(FALSE, NULL);		// switch to windowed mode

	// close and release all existing COM objects
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}

// this is the function used to render a single frame
void RenderFrame(void)
{
	// clear the back buffer to a deep blue
	devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f));

	// do 3D rendering on the bakc buffer here

	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}