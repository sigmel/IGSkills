#pragma once

#include <memory>

#include "Core/ApiExport.hpp"

class Window;
class IShader;
class IVertexBuffer;

class IRenderDevice
{
public:
	IRenderDevice() = default;
	virtual ~IRenderDevice() = default;

	virtual void Clear(float r, float g, float b, float depth) = 0;

	virtual void SetShader(IShader* shader) = 0;

	virtual void DrawTriangles(IVertexBuffer* vertexBuffer) = 0;

	virtual void StartInitResources() = 0;
	virtual void EndInitResources() = 0;

	virtual void Present() = 0;
	virtual void WaitForGpu() = 0;
	virtual void Reset() = 0;

	static std::unique_ptr<IRenderDevice> Make(Window* window);
};