#include <windows.h>
#include <windowsx.h>

// direct3D header files
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

// include to our project some Direct3D libraries
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

// global definitions, variables, COM-objects, etc.
#define SCREEN_WIDTH 800						// the screen width
#define SCREEN_HEIGHT 600						// the screen height

IDXGISwapChain *swapchain = nullptr;			// the swapchain
ID3D11Device *dev = nullptr;					// the device
ID3D11DeviceContext *devcon = nullptr;			// the device context
ID3D11RenderTargetView *backbuffer = nullptr;	// the render target view

ID3D11VertexShader *pVShader = nullptr;			// the vertex shader
ID3D11PixelShader  *pPShader = nullptr;			// the pixel shader

ID3D11Buffer *pVBuffer = nullptr;				// the vertex buffer

struct VERTEX									// a struct which represents the vertex
{
	FLOAT X, Y, Z;
	D3DXCOLOR Color;
};

ID3D11InputLayout *pLayout = nullptr;			// the input layout for the vertex buffer



// functions prototypes

void InitD3D(HWND hWnd);	// inialization of the swapchain, device, device context, viewport

void InitPipeline(void);	// initialization of the shaders, input layout
void InitGraphics(void);	// initialization of the shape, vertex buffer
void RenderFrame(void);		// this is the function for rendering of each frame into the screen


void ClearD3D(void);		// releasing all the COM-objects, used memory; doing some preparations before closing the window

// custom handler for events of the window
LRESULT CALLBACK WindowProc(HWND hWnd,
							UINT message,
							WPARAM wParam,
							LPARAM lParam);

// main function
int WINAPI WinMain(HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdLine,
					int nCmdShow)
{
	HWND hWnd;		// the window handler
	WNDCLASSEX wc;	// the description of a new window class

	ZeroMemory(&wc, sizeof(WNDCLASSEX));


	// creation of a description for our new window class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInstance;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wc.lpszClassName = L"WindowClass";

	// register this new window class
	RegisterClassEx(&wc);

	// adjust size parameters of the window
	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	// create a new window and use the result of this function as a window handler
	hWnd = CreateWindowEx(NULL,
							L"WindowClass",
							L"Our window program!",
							WS_OVERLAPPEDWINDOW,
							100,
							100,
							wr.right - wr.left,
							wr.bottom - wr.top,
							NULL,
							NULL,
							hInstance,
							NULL);

	// show the window which has been just created
	ShowWindow(hWnd, nCmdShow);

	// initialization of Direct3D
	InitD3D(hWnd);
	InitPipeline();
	InitGraphics();

	// main loop

	MSG msg;		// we'll put here messages about window events

	while (TRUE)	// infinite loop
	{
		// peek message from the queue about some event of the window 
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);		// translate the message to the proper format
			DispatchMessage(&msg);		// dispatch the message to the WindowProc() function

			if (msg.message == WM_QUIT)	// if we get message about closing of the window
			{
				break;					// then break the infinite loop
			}
		}
		else
		{
			// do here your game stuff
			RenderFrame();				// rendering each frame into the screen
		}
	}

	ClearD3D();							// doing some preparations before closing the window

	return msg.wParam;					// return to Windows the code about closing the window
}

// this is the custom handler for window events
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:			// message about closing the window
		{
			PostQuitMessage(0);		// pass to the main loop message about closing the window
			return 0;
		}
	}

	// the default hanlder for window events
	DefWindowProc(hWnd, message, wParam, lParam);
}

// inialization of the swapchain, device, device context, viewport
void InitD3D(HWND hWnd)
{
	// create a description for the swap chain
	DXGI_SWAP_CHAIN_DESC scd;
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

	// create the device, device context, swap chain using the swapchain description
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


	// create and initialize the render target output
	ID3D11Texture2D *pBackBuffer = nullptr;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);	// initialize the render target view with the pBackBuffer
	pBackBuffer->Release();											// we don't need pBackBuffer anymore

	devcon->OMSetRenderTargets(1, &backbuffer, NULL);				// set this backbuffer as active render target view



	// create and initialize the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	// the viewport description
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	devcon->RSSetViewports(1, &viewport);	// set active viewport
}


// initialization of the shaders, input layout
void InitPipeline(void)
{
	ID3D10Blob *VS = nullptr;	// a blob for the compiled code of the vertex shader
	ID3D10Blob *PS = nullptr;	// a blob for the compiled code of the pixel shader

	// compile the vertex shader code
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

	// compile the pixel shader code
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

	// encapsulate both shaders into the shader objects
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVShader);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPShader);

	// set these shaders as active
	devcon->VSSetShader(pVShader, NULL, NULL);
	devcon->PSSetShader(pPShader, NULL, NULL);



	// create and initialize the input layout
	D3D11_INPUT_ELEMENT_DESC ied[] =	// input elements description
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// create the input layout using the input elements description
	dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);

	// set the input layout for the vertex buffer
	devcon->IASetInputLayout(pLayout);
}

// initialization of the shape, vertex buffer
void InitGraphics(void)
{
	// create a triangle which we'll show on the screen
	VERTEX triangle[] =
	{
		{ 0.0f, 0.5f, 0.0f,     D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f) },
		{ 0.45f, -0.5f, 0.0f,   D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f) },
		{ -0.45f, -0.5f, 0.0f,  D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f) },
	};

	// create a description for the vertex buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));

	bd.ByteWidth = sizeof(VERTEX) * 3;			// the size of the vertex buffer = sizeof(VERTEX) * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// use the buffer as vertex buffer
	bd.Usage = D3D11_USAGE_DYNAMIC;				// we'll rewrite data in this buffer for the each frame
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// allow the CPU writing into this buffer

	dev->CreateBuffer(&bd, NULL, &pVBuffer);	// create a new vertex buffer using the buffer description


	// fill in the vertex buffer with data of the triangle
	D3D11_MAPPED_SUBRESOURCE ms;
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);	// locking of the vertex buffer during writing the data into it
	memcpy(ms.pData, triangle, sizeof(triangle));						// copying of the triangle's data into the mapped subresource of the vertex buffer
	devcon->Unmap(pVBuffer, NULL);										// unlocking of the vertex buffer
}

FLOAT colorVal = 1.0f;		// white color
FLOAT colorStride = 0.01;	// value of the color changing step
int tickCount = 0;			// seconds value

// this is the function for rendering each frame on the screen
void RenderFrame(void)
{
	// clear up and fill in the render target view with a deep blue color

	// each 0.1 second we change the color of the render target view
	if ((GetTickCount() / 100) != tickCount)
	{
		tickCount = GetTickCount() / 100;
		colorVal -= colorStride;

		devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(colorVal - 1.0f, colorVal - 0.8f, colorVal - 0.6f, 1.0f));
	}

	// rendering into the backbuffer (the render target view)
	UINT offset = 0;
	UINT stride = sizeof(VERTEX);
	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
	devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcon->Draw(3, 0);

	// swap values between the frontbuffer and the backbuffer
	swapchain->Present(0, 0);
}

// clears up Direct3D,
// releasing all the COM-objects, used memory; 
// doing some preparations before closing the window
void ClearD3D(void)
{
	swapchain->SetFullscreenState(false, NULL);

	// releasing all the global COM-objects
	pLayout->Release();
	pVBuffer->Release();
	pVShader->Release();
	pPShader->Release();
	backbuffer->Release();
	swapchain->Release();
	dev->Release();
	devcon->Release();
}
