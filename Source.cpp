#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine,
				   int nShowCmd)
{
	MessageBox(NULL,
				L"Hello, world!",
				L"Just another hello world program!",
				MB_ICONEXCLAMATION | MB_OK);

	return 0;
}