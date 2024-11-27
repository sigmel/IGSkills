#include "Render/Shader.hpp"

#if RENDERAPI_DIRECTX12
#include "DirectX12/DirectX12Shader.hpp"
#else
#error "No valid Render API defined"
#endif

std::unique_ptr<IShader> IShader::Make(IRenderDevice* device, const std::wstring& filename, const std::string& vertexFunction, const std::string& pixelFunction, const std::vector<std::string>& vertexDescription)
{
#if RENDERAPI_DIRECTX12
	return std::make_unique<DirectX12Shader>(device, filename, vertexFunction, pixelFunction, vertexDescription);
#else
	return nullptr;
#endif
}