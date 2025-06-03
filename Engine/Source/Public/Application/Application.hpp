#pragma once

#include <memory>

#include "Core/ApiExport.hpp"

class Window;
class IRenderDevice;
class Sprite;
class OrthographicCamera;
class FileManager;

class Application
{
public:
	LIBRARY_API Application();
	LIBRARY_API ~Application();

	LIBRARY_API void MakeWindow(void* instance, const char16_t* name, uint16_t width, uint16_t height);
	LIBRARY_API void MakeRenderDevice();

	LIBRARY_API void Run();

private:
	std::unique_ptr<FileManager> _fileManager;
	std::unique_ptr<Window> _window;
	std::unique_ptr<IRenderDevice> _renderDevice;
	std::unique_ptr<Sprite> _sprite;

	std::unique_ptr<OrthographicCamera> _camera;
};