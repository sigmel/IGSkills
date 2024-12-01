#include "Render/Sprite.hpp"

#include "Render/RenderDevice.hpp"
#include "Render/Buffer.hpp"
#include "Render/Shader.hpp"

/* can switch to this once we have texture support
static const IVertexBuffer::VertexPosUV kVertices[] = {
	{{0, 0, 0}, {0, 0}}, // bottom left
	{{0, 1, 0}, {0, 1}}, // top left
	{{1, 1, 0}, {1, 1}}, // top right
	{{0, 0, 0}, {0, 0}}, // bottom left
	{{1, 1, 0}, {1, 1}}, // top right
	{{1, 0, 0}, {1, 0}}, // bottom right
};
*/

static const IVertexBuffer::VertexPosColor kVertices[] = {
	{{0, 0, 0}, {1, 1, 1, 1}}, // bottom left
	{{0, 1, 0}, {1, 0, 0, 1}}, // top left
	{{1, 1, 0}, {0, 1, 0, 1}}, // top right
	{{0, 0, 0}, {1, 1, 1, 1}}, // bottom left
	{{1, 1, 0}, {0, 1, 0, 1}}, // top right
	{{1, 0, 0}, {0, 0, 1, 0}}, // bottom right
};

Sprite::Sprite(IRenderDevice* device, SkFloat2 position, SkFloat2 size):
	_position(position),
	_size(size),
	_vertexBuffer(IVertexBuffer::Make(device, kVertices, sizeof(kVertices), sizeof(IVertexBuffer::VertexPosColor))),
	_shader(IShader::Make(device, L"Content/Shaders/Sprite.hlsl", "VSMain", "PSMain", IVertexBuffer::VertexPosColor::kDescription))
{
}

Sprite::~Sprite()
{
}

void Sprite::Render(IRenderDevice* device)
{
	device->SetShader(_shader.get());
	device->DrawTriangles(_vertexBuffer.get());
}