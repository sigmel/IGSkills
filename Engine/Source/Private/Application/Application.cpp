#include "Application/Application.hpp"

#include <cassert>

#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

Application::Application()
{
}

Application::~Application()
{
}

void Application::MakeWindow(void* instance, wchar_t* name, uint16_t width, uint16_t height)
{
	_window = std::make_unique<Window>(instance, name, width, height);
}

void Application::MakeRenderDevice()
{
	assert(_window.get() != nullptr);
	_renderDevice = IRenderDevice::Make(_window.get());
}

void Application::Run()
{
	assert(_window.get() != nullptr);

	while (_window->ProcessMessages())
	{
		// @TODO: simplify this so we aren't putting so much implementation on the caller
		_renderDevice->Reset();
		_renderDevice->Clear(0, 0, 0, 1);
		_renderDevice->Present();
		_renderDevice->WaitForGpu();
	}
}