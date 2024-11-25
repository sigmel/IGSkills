#include "Windows.h"

#include <Application/Application.hpp>

int wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	Application app;
	app.MakeWindow(hInstance, L"Flappy Bird", 1280, 720);
	app.MakeRenderDevice();
	app.Run();
	return 0;
}