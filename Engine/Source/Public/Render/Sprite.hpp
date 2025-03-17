#pragma once

#include <memory>

#include "Math/Math.hpp"

#include "Core/ApiExport.hpp"

class IRenderDevice;
class IVertexBuffer;
class IConstantBuffer;
class IShader;
class ITexture;
class FileManager;

class Sprite
{
public:
	LIBRARY_API Sprite(IRenderDevice* device, FileManager* file, SkFloat2 position, SkFloat2 size);
	LIBRARY_API ~Sprite();

	LIBRARY_API void Render(IRenderDevice* device);

private:
	struct ConstantBuffer
	{
		SkMatrix _worldMatrix;
	};

	SkFloat2 _position;
	SkFloat2 _size;
	ConstantBuffer _spriteInfo;

	std::unique_ptr<IVertexBuffer> _vertexBuffer;
	std::unique_ptr<IConstantBuffer> _constantBuffer;
	std::unique_ptr<IShader> _shader;
	std::unique_ptr<ITexture> _texture;
};