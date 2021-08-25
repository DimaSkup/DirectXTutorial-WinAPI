// include the basic windows header files and
// the Direct3D header files
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// global declarations
IDXGISwapChain *swapchain = nullptr;		// the pointer to the swap chain interface
ID3D11Device *dev = nullptr;				// the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon = nullptr;		// the pointer to our Direct3D device context

// function prototypes
void InitD3D(HWND hWnd);		// sets up and initializes Direct3D
void CleanD3D(void);			// closes Direct3D and releases memory

// our custom handler for events of a window
LRESULT CALLBACK WindowProc(HWND hWnd,
							UINT message,
							WPARAM wParam,
							LPARAM lParam);

// main function
// there is everything takes the beginning
int WINAPI WinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	HWND hWnd;		// a handler of the window
	WNDCLASSEX wc;	// this is a structure which will have data about some window class

	ZeroMemory(&wc, sizeof(wc));	// fill in all "wc" structure object with NULL values

	// set up some properties of a future window class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wc.lpszClassName = L"WindowClass1";

	// register the window class
	RegisterClassEx(&wc);


	RECT wr = { 0, 0, 500, 400 };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);


	// create a new window and get as result a window handler value
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

	// show this new window
	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	InitD3D(hWnd);

	// main loop

	// a structure which will contain queue of messages about window events
	MSG msg = {NULL};

	// while we don't get some message about closing of the window
	// then this is an unending cycle
	while (TRUE)
	{
		// check to see if any messages are waiting in the queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			// translate the message into the right format
			TranslateMessage(&msg);

			// dispatch the message to our custom handler of window events
			// (to the WindowProc function)
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}
		else
		{
			// Run game code here
			// ...
			// ...
		}
	}

	// clean up DirectX and COM
	CleanD3D();

	// return to system a WM_QUIT code, it means that the window has been closed
	return msg.wParam;
}

// this is our cursom handler of window events
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// switch between message value to switch on our particular case
	switch (message)
	{
		// case: We pressed on a "close" button of the window
		case WM_DESTROY:
		{
			PostQuitMessage(0);		// return to main function a "window was closed" value
			return 0;		
		}
	}

	// if we don't have some particular message value, then we pass it to a standard handler of window events
	DefWindowProc(hWnd, message, wParam, lParam);
}



// LAUNCHING Direct3D

// this function intializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;								// one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// how swap chain is to be used
	scd.OutputWindow = hWnd;							// the window to be used
	scd.SampleDesc.Count = 4;							// how many multisamples
	scd.Windowed = TRUE;								// windowed/full-screen mode

	// create a device, device context and swap chain
	// using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,		// *pAdapter
									D3D_DRIVER_TYPE_HARDWARE, // driver type
									NULL,	// software
									D3D11_CREATE_DEVICE_DEBUG,	// flags
									NULL,	// *pFeatureLevels
									NULL,	// Feature Levels (how many)
									D3D11_SDK_VERSION,	// SDKVersion
									&scd,	// pointer to swap chain description
									&swapchain,	// pointer to pointer to swap chain
									&dev,	// pointer to pointer to device
									NULL,	// pointer to a feature level variable
									&devcon);	// pointer to pointer to device context
}

// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
	// close and release all existing COM objects
	//swapchain->Release();
	//dev->Release();
	//devcon->Release();
}