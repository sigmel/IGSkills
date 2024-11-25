#pragma once

#include <thread>

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include "Render/RenderDevice.hpp"

class DirectX12Device final : public IRenderDevice
{
public:
	DirectX12Device(Window* window);

    void SetThread() { _threadId = std::this_thread::get_id(); }

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

    void RunCommandList(ID3D12GraphicsCommandList* commandList)
    {
        ID3D12CommandList* ppCommandLists[] = { commandList };
        _commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }

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
    uint32_t _currentBackBufferIndex = 0;

    Microsoft::WRL::ComPtr<ID3D12Fence> _fence;
    uint64_t _frameFenceValues[NUM_BACK_BUFFERS] = { 0 };
    HANDLE _fenceEvent = nullptr;

    std::thread::id _threadId;

    bool _fullscreen = false;

    void UpdateRenderTargetViews();
};