#include "DirectX12Device.hpp"

#include "Window/Window.hpp"

using namespace std;
using namespace Microsoft::WRL;

DirectX12Device::DirectX12Device(Window* window)
{
    SetThread();

    uint32_t createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

    ComPtr<ID3D12Debug> debugInterface;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
    debugInterface->EnableDebugLayer();
#endif

    // Choose the adapter that has the most dedicated video memory, this is likely a discrete GPU which we prefer
    CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&_factory));
    size_t maxDedicatedVideoMemory = 0;
    ComPtr<IDXGIAdapter1> adapter;
    for (uint32_t adapterIndex = 0; _factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND; adapterIndex++)
    {
        DXGI_ADAPTER_DESC1 adapterDesc;
        adapter->GetDesc1(&adapterDesc);

        if ((adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
            SUCCEEDED(D3D12CreateDevice(_adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)) &&
            adapterDesc.DedicatedVideoMemory > maxDedicatedVideoMemory)
        {
            maxDedicatedVideoMemory = adapterDesc.DedicatedVideoMemory;
            _adapter = adapter;
        }
    }

    D3D12CreateDevice(_adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&_device));

#if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(_device.As(&infoQueue)))
    {
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

        D3D12_MESSAGE_SEVERITY severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        D3D12_MESSAGE_ID denyIds[] =
        {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
        };

        D3D12_INFO_QUEUE_FILTER newFilter = {};
        newFilter.DenyList.NumSeverities = _countof(severities);
        newFilter.DenyList.pSeverityList = severities;
        newFilter.DenyList.NumIDs = _countof(denyIds);
        newFilter.DenyList.pIDList = denyIds;
        infoQueue->PushStorageFilter(&newFilter);
    }
#endif

    D3D12_COMMAND_QUEUE_DESC commandDesc = {};
    commandDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    commandDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    _device->CreateCommandQueue(&commandDesc, IID_PPV_ARGS(&_commandQueue));

    RECT clientRect;
    GetClientRect(window->GetWindow(), &clientRect);

    _scissorRect = clientRect;
    _viewport.TopLeftX = 0.0f;
    _viewport.TopLeftY = 0.0f;
    _viewport.Width = static_cast<float>(clientRect.right - clientRect.left);
    _viewport.Height = static_cast<float>(clientRect.bottom - clientRect.top);
    _viewport.MinDepth = D3D12_MIN_DEPTH;
    _viewport.MaxDepth = D3D12_MAX_DEPTH;

    DXGI_SWAP_CHAIN_DESC1 swapDesc = {};
    swapDesc.Width = clientRect.right - clientRect.left;
    swapDesc.Height = clientRect.bottom - clientRect.top;
    swapDesc.Format = BACK_BUFFER_FORMAT;
    swapDesc.SampleDesc = { 1, 0 };
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.BufferCount = NUM_BACK_BUFFERS;
    swapDesc.Scaling = DXGI_SCALING_STRETCH;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ComPtr<IDXGISwapChain1> swapChain;
    _factory->CreateSwapChainForHwnd(_commandQueue.Get(), window->GetWindow(), &swapDesc, nullptr, nullptr, &swapChain);
    _factory->MakeWindowAssociation(window->GetWindow(), DXGI_MWA_NO_ALT_ENTER);
    swapChain.As(&_swapChain);

    D3D12_DESCRIPTOR_HEAP_DESC descriptorDesc = {};
    descriptorDesc.NumDescriptors = NUM_BACK_BUFFERS;
    descriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    descriptorDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    _device->CreateDescriptorHeap(&descriptorDesc, IID_PPV_ARGS(&_rtvHeap));

    descriptorDesc.NumDescriptors = 1;
    descriptorDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    _device->CreateDescriptorHeap(&descriptorDesc, IID_PPV_ARGS(&_dsvHeap));

    Resize(swapDesc.Width, swapDesc.Height);

    for (uint32_t bufferIndex = 0; bufferIndex < NUM_BACK_BUFFERS; bufferIndex++)
    {
        _device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_commandAllocator[bufferIndex]));
    }
    _device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _commandAllocator[_currentBackBufferIndex].Get(), nullptr, IID_PPV_ARGS(&_commandList));
    _commandList->Close();

    _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void DirectX12Device::Present()
{
    // this will present the backbuffer when it is able

    assert(_threadId == this_thread::get_id());

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = _backBuffers[_currentBackBufferIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    _commandList->ResourceBarrier(1, &barrier);
    _commandList->Close();

    ID3D12CommandList* const commandLists[] = { _commandList.Get() };
    _commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

    _swapChain->Present(1, 0);

    uint64_t currentValue = _frameFenceValues[_currentBackBufferIndex];
    _commandQueue->Signal(_fence.Get(), currentValue);

    _currentBackBufferIndex = _swapChain->GetCurrentBackBufferIndex();

    uint64_t completedValue = _fence->GetCompletedValue();
    while (completedValue < _frameFenceValues[_currentBackBufferIndex])
    {
        _fence->SetEventOnCompletion(_frameFenceValues[_currentBackBufferIndex], _fenceEvent);
        WaitForSingleObject(_fenceEvent, INFINITE);

        completedValue = _fence->GetCompletedValue();
    }

    // Set the fence value for the next frame.
    _frameFenceValues[_currentBackBufferIndex] = currentValue + 1;
}

void DirectX12Device::WaitForGpu()
{
    // this will wait for the gpu to be idle

    assert(_threadId == this_thread::get_id());

    uint64_t currentValue = _frameFenceValues[_currentBackBufferIndex];
    _commandQueue->Signal(_fence.Get(), currentValue);
    _fence->SetEventOnCompletion(currentValue, _fenceEvent);

    uint64_t completedValue = _fence->GetCompletedValue();
    while (completedValue < currentValue)
    {
        WaitForSingleObject(_fenceEvent, INFINITE);
        completedValue = _fence->GetCompletedValue();
    }

    _frameFenceValues[_currentBackBufferIndex] = currentValue + 1;
}

void DirectX12Device::Reset()
{
    // this wil reset the device for the next frame

    assert(_threadId == this_thread::get_id());

    _commandAllocator[_currentBackBufferIndex]->Reset();
    _commandList->Reset(_commandAllocator[_currentBackBufferIndex].Get(), nullptr);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = _backBuffers[_currentBackBufferIndex].Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    _commandList->ResourceBarrier(1, &barrier);

    _commandList->RSSetViewports(1, &_viewport);
    _commandList->RSSetScissorRects(1, &_scissorRect);

    auto rtv = GetRenderTargetView();
    auto dsv = GetDepthStencilView();
    _commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
}

void DirectX12Device::Clear(float r, float g, float b, float depth)
{
    // clear the backbuffers and depth buffer to the specified values

    assert(_threadId == this_thread::get_id());

    FLOAT clearColor[] = { r, g, b, 1.0f };

    auto rtv = GetRenderTargetView();
    _commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

    auto dsv = GetDepthStencilView();
    _commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
}

void DirectX12Device::Resize(uint32_t width, uint32_t height)
{
    // this will recreate the backbuffers and depth stencil buffer at the specified size

    assert(_threadId == this_thread::get_id());

    for (uint32_t bufferIndex = 0; bufferIndex < NUM_BACK_BUFFERS; bufferIndex++)
    {
        _backBuffers[bufferIndex].Reset();
        _frameFenceValues[bufferIndex] = _frameFenceValues[_currentBackBufferIndex];
    }
    _depthBuffer.Reset();

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    _swapChain->GetDesc(&swapDesc);
    _swapChain->ResizeBuffers(NUM_BACK_BUFFERS, width, height, BACK_BUFFER_FORMAT, swapDesc.Flags);

    _scissorRect.right = width;
    _scissorRect.bottom = height;
    _viewport.Width = static_cast<float>(width);
    _viewport.Height = static_cast<float>(height);

    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProperties.CreationNodeMask = 1;
    heapProperties.VisibleNodeMask = 1;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optimizedClearValue = {};
    optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
    optimizedClearValue.DepthStencil = { 1.0f, 0 };

    _device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &optimizedClearValue,
        IID_PPV_ARGS(&_depthBuffer)
    );

    _currentBackBufferIndex = _swapChain->GetCurrentBackBufferIndex();
    UpdateRenderTargetViews();
}

void DirectX12Device::UpdateRenderTargetViews()
{
    // this will update the render target views for the backbuffers and the depth buffer

    assert(_threadId == this_thread::get_id());

    uint32_t descriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    auto descriptorHandle = _rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (uint32_t bufferIndex = 0; bufferIndex < NUM_BACK_BUFFERS; bufferIndex++)
    {
        _swapChain->GetBuffer(bufferIndex, IID_PPV_ARGS(&_backBuffers[bufferIndex]));
        _device->CreateRenderTargetView(_backBuffers[bufferIndex].Get(), nullptr, descriptorHandle);
        descriptorHandle.ptr += descriptorSize;
    }

    descriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
    dsv.Format = DXGI_FORMAT_D32_FLOAT;
    dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

    _device->CreateDepthStencilView(_depthBuffer.Get(), &dsv, _dsvHeap->GetCPUDescriptorHandleForHeapStart());
}