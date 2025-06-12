#pragma once

#include <memory>
#include <string>

#include "Core/ApiExport.hpp"
#include "Math/Math.hpp"

class IRenderDevice;
class IVertexBuffer;
class IConstantBuffer;
class IShader;
class ITexture;
class FileManager;

class Sprite
{
public:
	LIBRARY_API Sprite(IRenderDevice* device, FileManager* file, const std::u16string& filename, SkFloat2 position, SkFloat2 size);
	LIBRARY_API ~Sprite();

	LIBRARY_API void Render(IRenderDevice* device);

private:
	SkFloat2 _position;
	SkFloat2 _size;
	SkMatrix _worldMatrix;

	std::unique_ptr<IVertexBuffer> _vertexBuffer;
	std::unique_ptr<IShader> _shader;
	std::unique_ptr<ITexture> _texture;
};