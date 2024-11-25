#include "Render/RenderDevice.hpp"

#include "Window/Window.hpp"
#include "DirectX12/DirectX12Device.hpp"

std::unique_ptr<IRenderDevice> IRenderDevice::Make(Window* window)
{
	return std::make_unique<DirectX12Device>(window);
}