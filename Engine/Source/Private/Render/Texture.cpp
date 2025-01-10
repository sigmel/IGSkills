#include "Texture.hpp"

#if RENDERAPI_DIRECTX12
#include "DirectX12/DirectX12Texture.hpp"
#else
#error "No valid Render API defined"
#endif

std::unique_ptr<ITexture> ITexture::Make(IRenderDevice* device, IFile* file)
{
#if RENDERAPI_DIRECTX12
	return std::make_unique<DirectX12Texture>(device, file);
#else
	return nullptr;
#endif
}