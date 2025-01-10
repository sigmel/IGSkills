#pragma once

#include <wrl.h>
#include <d3d12.h>

#include "Render/Texture.hpp"

class DirectX12Texture final : public ITexture
{
public:
	DirectX12Texture(IRenderDevice* device, IFile* file);
	virtual ~DirectX12Texture();

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> _texture;
	Microsoft::WRL::ComPtr<ID3D12Resource> _uploadTexture;
};