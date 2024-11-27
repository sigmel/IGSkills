#pragma once

#include <string>
#include <vector>

#include <d3d12.h>
#include <wrl.h>

#include "Render/Shader.hpp"

class DirectX12Shader final : public IShader
{
public:
	DirectX12Shader(IRenderDevice* device, const std::wstring& filename, const std::string& vertexFunction, const std::string& pixelFunction, const std::vector<std::string>& vertexDescription);

private:
    Microsoft::WRL::ComPtr<ID3DBlob> _vertexShaderBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> _pixelShaderBlob;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> _rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> _pipelineState;
};