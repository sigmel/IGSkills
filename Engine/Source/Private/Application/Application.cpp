#include "Application/Application.hpp"

#include <cassert>

Application::Application()
{
}

void Application::MakeWindow(HINSTANCE hInstance, wchar_t* name, uint16_t width, uint16_t height)
{
	_window = std::make_unique<Window>(hInstance, name, width, height);
}

void Application::Run()
{
	assert(_window.get() != nullptr);

	while (_window->ProcessMessages())
	{
	}
}