#include "Render/RenderDevice.hpp"

#include "Window/Window.hpp"

#if RENDERAPI_DIRECTX12
#include "DirectX12/DirectX12Device.hpp"
#else
#error "No valid Render API defined"
#endif

std::unique_ptr<IRenderDevice> IRenderDevice::Make(Window* window)
{
#if RENDERAPI_DIRECTX12
	return std::make_unique<DirectX12Device>(window);
#else
	return nullptr;
#endif
}