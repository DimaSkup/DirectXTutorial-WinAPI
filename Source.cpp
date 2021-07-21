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

ID3D11VertexShader *pVS;		// the vertex shader
ID3D11PixelShader  *pPS;		// the pixel shader

// a struct to define a vertex
struct VERTEX
{
	FLOAT X, Y, Z;
	D3DXCOLOR Color;
};

ID3D11Buffer *pVBuffer;			// a pointer to the vertex buffer
ID3D11InputLayout *pLayout;		// the input layout

// function prototypes
void InitD3D(HWND hWnd);		// sets up and initializes Direct3D
void CleanD3D(void);			// closes Direct3D and releases memory
void RenderFrame(void);			// used to render a single frame
void InitGraphics(void);    // creates the shape to render
void InitPipeline(void);    // loads and prepares the shaders


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
						  10,		// x-position of the window
						  10,		// y-position of the window
						  SCREEN_WIDTH,		// width of the window
						  SCREEN_HEIGHT,	// height of the window
						  NULL,		// we have no parent window, NULL
						  NULL,		// we aren't using menus, NULL
						  hInstance,	// application handle
						  NULL);	// used with multiple windows, NULL
	
	// display the window on the screen
	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	InitD3D(hWnd);

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

		RenderFrame();
	}

	// clean up DirectX and COM
	CleanD3D();
	
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

	InitPipeline();
	InitGraphics();

}

// this is the function that cleans up Direct3D and COM
void CleanD3D()
{
	swapchain->SetFullscreenState(FALSE, NULL);		// switch to windowed mode

	// close and release all existing COM objects
	pLayout->Release();
	pVS->Release();
	pPS->Release();
	pVBuffer->Release();
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

	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

	// select which primitive type we are using
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	devcon->Draw(3, 0);

	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}

// this is the function that initializes the pipeline
void InitPipeline()
{
	// load and compile the two shaders
	ID3D10Blob *VS;
	ID3D10Blob *PS;

	D3DX11CompileFromFile(L"shaders.shader", NULL, NULL,
						  "VShader", "vs_4_0", NULL, NULL, NULL,
						  &VS, NULL, NULL);
	D3DX11CompileFromFile(L"shaders.shader", NULL, NULL, 
						  "PShader", "ps_4_0", NULL, NULL, NULL,
						  &PS, NULL, NULL);

	// encapsulate both shaders into shader objects
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	// set the shader objects
	devcon->VSSetShader(pVS, nullptr, NULL);
	devcon->PSSetShader(pPS, nullptr, NULL);

	// create an input elements description array
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	// initialize the Input Layout
	dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}

void InitGraphics()
{
	// create a triangle using the VERTEX struct
	VERTEX OurVertices[] =
	{
		{0.0f, 0.5f, 0.0f, D3DXCOLOR{1.0f, 0.0f, 0.0f, 1.0f}},
		{0.45f, -0.5f, 0.0f, D3DXCOLOR{0.0f, 1.0f, 0.0f, 1.0f}},
		{-0.45f, -0.5f, 0.0f, D3DXCOLOR{0.0f, 0.0f, 1.0f, 1.0f}}
	};

	// create the vertex buffer description
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;				// write access access by CPU and GPU
	bd.ByteWidth = sizeof(VERTEX) * 3;			// size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// allow CPU to write in buffer

	// create the vertex buffer
	dev->CreateBuffer(&bd, NULL, &pVBuffer);

	// copy the vertices into the vertex buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);	// map the buffer
	memcpy(ms.pData, OurVertices, sizeof(OurVertices));					// copy the data
	devcon->Unmap(pVBuffer, NULL);										// unmap the buffer
}