#pragma once

#include <memory>

#include "Math/Math.hpp"

#include "apiexport.hpp"

class IRenderDevice;
class IVertexBuffer;
class IShader;

class Sprite
{
public:
	LIBRARY_API Sprite(IRenderDevice* device, SkFloat2 position, SkFloat2 size);
	LIBRARY_API ~Sprite();

	LIBRARY_API void Render(IRenderDevice* device);

private:
	SkFloat2 _position;
	SkFloat2 _size;

	std::unique_ptr<IVertexBuffer> _vertexBuffer;
	std::unique_ptr<IShader> _shader;
};