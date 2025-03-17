#pragma once

#include <vector>
#include <cassert>

#include <wrl.h>
#include <d3d12.h>

#include "Render/Buffer.hpp"

#include "DirectX12Device.hpp"

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

class DirectX12ConstantBuffer final : public IConstantBuffer
{
public:
    DirectX12ConstantBuffer(IRenderDevice* device, size_t size);

    void Update(IRenderDevice* device, void* data, size_t size) override;

    D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress(uint32_t bufferIndex) { assert(bufferIndex < _countof(_uploadConstantBuffer)); return _uploadConstantBuffer[bufferIndex]->GetGPUVirtualAddress(); }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> _uploadConstantBuffer[DirectX12Device::GetNumBackBuffers()];
    uint8_t* _uploadData[DirectX12Device::GetNumBackBuffers()];
};