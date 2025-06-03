#pragma once

#include <thread>

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#include "Render/RenderDevice.hpp"

class DirectX12Device final : public IRenderDevice
{
public:
	DirectX12Device(Window* window);

	ID3D12Device* GetDevice() { return _device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return _commandList.Get(); }

	void SetThread() { _threadId = std::this_thread::get_id(); }

	void SetShader(IShader* shader) override;
	void SetCamera(ICamera* camera) override;
	void ApplyCamera() override;
	void SetConstantData(void* data, size_t size) override;
	void SetConstantBuffer(IConstantBuffer* constantBuffer) override;

	void DrawTriangles(IVertexBuffer* vertexBuffer) override;

	void StartInitResources() override;
	void EndInitResources() override;

	void Present() override;
	void WaitForGpu() override;
	void Reset() override;

	void Clear(float r, float g, float b, float depth) override;

	void Resize(uint32_t width, uint32_t height);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView()
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtv = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
		uint32_t descriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		rtv.ptr += static_cast<uintptr_t>(descriptorSize) * _currentBackBufferIndex;
		return rtv;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView()
	{
		D3D12_CPU_DESCRIPTOR_HANDLE dsv = _dsvHeap->GetCPUDescriptorHandleForHeapStart();
		return dsv;
	}

	ID3D12DescriptorHeap* GetSRVHeap() { return _srvHeap.Get(); }

	static constexpr uint32_t GetNumBackBuffers() { return NUM_BACK_BUFFERS; }
	uint32_t GetCurrentBackBufferIndex() const { return _currentBackBufferIndex; }

private:
	static constexpr uint32_t NUM_BACK_BUFFERS = 2;
	static constexpr DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

	D3D12_VIEWPORT _viewport;
	D3D12_RECT _scissorRect;

	Microsoft::WRL::ComPtr<IDXGIFactory4> _factory;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> _adapter;
	Microsoft::WRL::ComPtr<ID3D12Device2> _device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> _commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> _swapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource> _backBuffers[NUM_BACK_BUFFERS];
	Microsoft::WRL::ComPtr<ID3D12Resource> _depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> _commandList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _commandAllocator[NUM_BACK_BUFFERS];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _srvHeap;
	uint32_t _currentBackBufferIndex = 0;

	Microsoft::WRL::ComPtr<ID3D12Fence> _fence;
	uint64_t _frameFenceValues[NUM_BACK_BUFFERS] = { 0 };
	HANDLE _fenceEvent = nullptr;

	std::thread::id _threadId;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> _resourceCommandAllocator;

	bool _fullscreen = false;

	ICamera* _currentCamera = nullptr;

	void UpdateRenderTargetViews();
};