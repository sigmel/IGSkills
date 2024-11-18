#pragma once

#include <cstdint>

#include <windows.h>

#include "apiexport.hpp"

class LIBRARY_API Window
{
public:
	Window(HINSTANCE hInstance, wchar_t* name, uint16_t width, uint16_t height);
	~Window();

	bool ProcessMessages();

private:
	HWND _hWnd = 0;
};