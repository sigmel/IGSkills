#include "DirectX12Shader.hpp"

#include <unordered_map>

#include <d3dcompiler.h>

#include "Render/Buffer.hpp"
#include "Render/DirectX12/DirectX12Device.hpp"

static const std::unordered_map<std::string, DXGI_FORMAT> kVertexInfoMap = {
	{IVertexBuffer::kPosition, DXGI_FORMAT_R32G32B32_FLOAT},
	{IVertexBuffer::kColor, DXGI_FORMAT_R32G32B32A32_FLOAT},
	{IVertexBuffer::kUV, DXGI_FORMAT_R32G32_FLOAT},
	{IVertexBuffer::kNormal, DXGI_FORMAT_R32G32B32_FLOAT}
};

DirectX12Shader::DirectX12Shader(IRenderDevice* device, const std::u16string& filename, const std::string& vertexFunction, const std::string& pixelFunction, const std::vector<std::string>& vertexDescription)
{
	DirectX12Device* dx12Device = static_cast<DirectX12Device*>(device);

#if defined(_DEBUG)
	uint32_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	uint32_t compileFlags = 0;
#endif
	
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	// shaders
	HRESULT result = D3DCompileFromFile(reinterpret_cast<const wchar_t*>(filename.c_str()), nullptr, nullptr, vertexFunction.c_str(), "vs_5_1", compileFlags, 0, &_vertexShaderBlob, &errorBlob);
	if (FAILED(result))
	{
		OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	}
	result = D3DCompileFromFile(reinterpret_cast<const wchar_t*>(filename.c_str()), nullptr, nullptr, pixelFunction.c_str(), "ps_5_1", compileFlags, 0, &_pixelShaderBlob, &errorBlob);
	if (FAILED(result))
	{
		OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	}

	// root signature
	// @TODO: Figure out a way to make this more programmatic as it only supports a single type of shader at the moment
	D3D12_DESCRIPTOR_RANGE1 descriptorRange = {
		.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
		.NumDescriptors = 1,
		.BaseShaderRegister = 0
	};
	D3D12_ROOT_PARAMETER1 rootParameter = {
		.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
		.DescriptorTable = {
			.NumDescriptorRanges = 1,
			.pDescriptorRanges = &descriptorRange,
		},
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,
	};
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {
		.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
		.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
		.MaxLOD = D3D12_FLOAT32_MAX,
		.ShaderRegister = 0,
		.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,
	};
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription = {
		.Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
		.Desc_1_1 = {
			.NumParameters = 1,
			.pParameters = &rootParameter,
			.NumStaticSamplers = 1,
			.pStaticSamplers = &samplerDesc,
			.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
					D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS,
		},
	};

	Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
	D3D12SerializeVersionedRootSignature(&rootSignatureDescription, &rootSignatureBlob, &errorBlob);
	dx12Device->GetDevice()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(), rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));

	const D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc =
	{
		FALSE,FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL,
	};

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputDesc;
	inputDesc.reserve(vertexDescription.size());
	for (auto& descElement : vertexDescription)
	{
		DXGI_FORMAT format = kVertexInfoMap.at(descElement);
		inputDesc.emplace_back(D3D12_INPUT_ELEMENT_DESC{ descElement.c_str(), 0, format, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 });
	}

	// pipeline state object
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc = {
		.pRootSignature = _rootSignature.Get(),
		.VS = { _vertexShaderBlob->GetBufferPointer(), _vertexShaderBlob->GetBufferSize() },
		.PS = { _pixelShaderBlob->GetBufferPointer(), _pixelShaderBlob->GetBufferSize() },
		.BlendState = {
			.RenderTarget = { renderTargetBlendDesc },
		},
		.SampleMask = UINT_MAX,
		.RasterizerState = {
			.FillMode = D3D12_FILL_MODE_SOLID,
			.CullMode = D3D12_CULL_MODE_BACK,
			.DepthClipEnable = true,
		},
		.DepthStencilState = {
			.DepthEnable = true,
			.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
			.DepthFunc = D3D12_COMPARISON_FUNC_LESS,
		},
		.InputLayout = { inputDesc.data(), static_cast<uint32_t>(inputDesc.size()) },
		.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		.NumRenderTargets = 1,
		.RTVFormats = { DXGI_FORMAT_R8G8B8A8_UNORM },
		.DSVFormat = DXGI_FORMAT_D32_FLOAT,
		.SampleDesc = {
			.Count = 1,
		},
	};

	dx12Device->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&_pipelineState));
}