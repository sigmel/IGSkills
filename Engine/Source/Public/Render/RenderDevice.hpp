#pragma once

#include <memory>

#include "apiexport.hpp"

class Window;

class IRenderDevice
{
public:
	IRenderDevice() = default;
	virtual ~IRenderDevice() = default;

	virtual void Clear(float r, float g, float b, float depth) = 0;

	virtual void Present() = 0;
	virtual void WaitForGpu() = 0;
	virtual void Reset() = 0;

	static std::unique_ptr<IRenderDevice> Make(Window* window);
};