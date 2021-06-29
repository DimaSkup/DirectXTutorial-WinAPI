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

// global declarations
IDXGISwapChain *swapchain;		// the pointer to the swap chain interface
ID3D11Device *dev;				// the pointer to the our Direct3D device interface
ID3D11DeviceContext *devcon;	// the pointer to the our Direct3D device context

// function prototypes
void InitD3D(HWND hWnd);		// sets up and initializes Direct3D
void CleanD3D(void);			// closes Direct3D and releases memory


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
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
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
						  wr.right - wr.left,		// width of the window
						  wr.bottom - wr.top,		// height of the window
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
			if (msg.message = WM_QUIT)
				break;
		}
		else
		{
			// Run game code here
			// ...
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
void Init3D(HWND hWnd)
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;									// one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		// use 32-bit colour
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// how swap chain is to be used
	scd.OutputWindow = hWnd;								// the window to be used
	scd.SampleDesc.Count = 4;								// how many multisamples
	scd.Windowed = TRUE;									// windowed/full-screen mode

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
}