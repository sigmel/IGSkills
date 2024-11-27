#include "DirectX12Buffer.hpp"

#include <unordered_map>

#include "Render/DirectX12/DirectX12Device.hpp"

DirectX12VertexBuffer::DirectX12VertexBuffer(IRenderDevice* device, const void* data, size_t dataSize, size_t stride)
{
	DirectX12Device* dx12Device = static_cast<DirectX12Device*>(device);

	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = dataSize;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	dx12Device->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer)
	);

	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	dx12Device->GetDevice()->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_uploadVertexBuffer)
	);

	BYTE* uploadData = nullptr;
	_uploadVertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&uploadData));
	memcpy(uploadData, data, dataSize);
	_uploadVertexBuffer->Unmap(0, nullptr);

	dx12Device->GetCommandList()->CopyBufferRegion(_vertexBuffer.Get(), 0, _uploadVertexBuffer.Get(), 0, dataSize);

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = _vertexBuffer.Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	dx12Device->GetCommandList()->ResourceBarrier(1, &barrier);

	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.SizeInBytes = static_cast<uint32_t>(dataSize);
	_vertexBufferView.StrideInBytes = static_cast<uint32_t>(stride);

}