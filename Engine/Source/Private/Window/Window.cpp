#include "Window/Window.hpp"

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window(HINSTANCE hInstance, wchar_t* name, uint16_t width, uint16_t height)
{
	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WndProc;
	windowClass.hInstance = hInstance;
	windowClass.hIcon = LoadIcon(hInstance, nullptr);
	windowClass.hIconSm = LoadIcon(hInstance, nullptr);
	windowClass.hCursor = LoadCursor(hInstance, IDC_ARROW);
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	windowClass.lpszClassName = name;
	RegisterClassEx(&windowClass);

	RECT windowRect = { 0, 0, width, height };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	uint32_t screenWidth = GetSystemMetrics(SM_CXSCREEN);
	uint32_t screenHeight = GetSystemMetrics(SM_CYSCREEN);
	uint32_t windowWidth = windowRect.right - windowRect.left;
	uint32_t windowHeight = windowRect.bottom - windowRect.top;

	uint32_t windowX = (screenWidth - windowWidth) / 2;
	uint32_t windowY = (screenHeight - windowHeight) / 2;

	_hWnd = CreateWindow(
		name,
		name,
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	ShowWindow(_hWnd, SW_SHOW);
}

Window::~Window()
{
}

bool Window::ProcessMessages()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}