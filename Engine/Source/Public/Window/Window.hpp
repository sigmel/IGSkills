#pragma once

#include <cstdint>

#include <windows.h>

#include "Core/ApiExport.hpp"

class Window
{
public:
	Window(void* instance, wchar_t* name, uint16_t width, uint16_t height);
	~Window();

	bool ProcessMessages();

	HWND GetWindow() { return _hWnd; }

private:
	HWND _hWnd = 0;
};