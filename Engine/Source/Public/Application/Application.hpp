#pragma once

#include <memory>

#include "Window/Window.hpp"

class LIBRARY_API Application
{
public:
	Application();

	void MakeWindow(HINSTANCE hInstance, wchar_t* name, uint16_t width, uint16_t height);

	void Run();

private:
	std::unique_ptr<Window> _window;
};