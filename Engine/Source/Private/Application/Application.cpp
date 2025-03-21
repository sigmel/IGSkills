#include "Application/Application.hpp"

#include <cassert>

#include "File/File.hpp"
#include "Window/Window.hpp"
#include "Render/RenderDevice.hpp"

#include "Render/Sprite.hpp"

Application::Application()
{
	_fileManager = std::make_unique<FileManager>();
}

Application::~Application()
{
}

void Application::MakeWindow(void* instance, const char16_t* name, uint16_t width, uint16_t height)
{
	_window = std::make_unique<Window>(instance, name, width, height);
}

void Application::MakeRenderDevice()
{
	assert(_window.get() != nullptr);
	_renderDevice = IRenderDevice::Make(_window.get());

	_renderDevice->StartInitResources();
	_sprite = std::make_unique<Sprite>(_renderDevice.get(), _fileManager.get(), SkFloat2(0, 0), SkFloat2(0, 0));
	_renderDevice->EndInitResources();
}

void Application::Run()
{
	assert(_window.get() != nullptr);

	while (_window->ProcessMessages())
	{
		// @TODO: simplify this so we aren't putting so much implementation on the caller
		_renderDevice->Reset();
		_renderDevice->Clear(0, 0, 0, 1);
		_sprite->Render(_renderDevice.get());
		_renderDevice->Present();
		_renderDevice->WaitForGpu();
	}
}