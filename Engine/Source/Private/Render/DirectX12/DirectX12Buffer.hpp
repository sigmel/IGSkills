#pragma once

#include <vector>

#include <wrl.h>
#include <d3d12.h>

#include "Render/Buffer.hpp"

class DirectX12VertexBuffer final : public IVertexBuffer
{
public:
    DirectX12VertexBuffer(IRenderDevice* device, const void* data, size_t dataSize, size_t stride);

    const D3D12_VERTEX_BUFFER_VIEW* GetView() { return &_vertexBufferView; }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> _vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> _uploadVertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;

    std::vector<D3D12_INPUT_ELEMENT_DESC> _inputDesc;
};