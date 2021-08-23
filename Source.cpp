#include <windows.h>
#include <windowsx.h>


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
		}
		else
		{
			// Run game code here
			// ...
			// ...
		}
	}

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