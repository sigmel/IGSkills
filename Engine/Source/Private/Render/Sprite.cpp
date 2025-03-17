#include "Render/Sprite.hpp"

#include "Render/RenderDevice.hpp"
#include "Render/Buffer.hpp"
#include "Render/Shader.hpp"
#include "Render/Texture.hpp"

#include "File/File.hpp"

static const IVertexBuffer::VertexPosUV kVertices[] = {
	{{0, 0, 0}, {0, 1}}, // bottom left
	{{0, 1, 0}, {0, 0}}, // top left
	{{1, 1, 0}, {1, 0}}, // top right
	{{0, 0, 0}, {0, 1}}, // bottom left
	{{1, 1, 0}, {1, 0}}, // top right
	{{1, 0, 0}, {1, 1}}, // bottom right
};

/* Keep around for a bit
static const IVertexBuffer::VertexPosColor kVertices[] = {
	{{0, 0, 0}, {1, 1, 1, 1}}, // bottom left
	{{0, 1, 0}, {1, 0, 0, 1}}, // top left
	{{1, 1, 0}, {0, 1, 0, 1}}, // top right
	{{0, 0, 0}, {1, 1, 1, 1}}, // bottom left
	{{1, 1, 0}, {0, 1, 0, 1}}, // top right
	{{1, 0, 0}, {0, 0, 1, 0}}, // bottom right
};
*/

Sprite::Sprite(IRenderDevice* device, FileManager* file, SkFloat2 position, SkFloat2 size):
	_position(position),
	_size(size),
	_vertexBuffer(IVertexBuffer::Make(device, kVertices, sizeof(kVertices), sizeof(IVertexBuffer::VertexPosUV))),
	_constantBuffer(IConstantBuffer::Make(device, sizeof(ConstantBuffer))),
	_shader(IShader::Make(device, u"Content/Shaders/Sprite.hlsl", "VSMain", "PSMain", IVertexBuffer::VertexPosUV::kDescription)),
	_texture(ITexture::Make(device, file->OpenFile(u"Content/Sprites/FlappyBirdSingle.png").get()))
{
	_spriteInfo._worldMatrix = SkIdentityMatrix();
}

Sprite::~Sprite()
{
}

void Sprite::Render(IRenderDevice* device)
{
	device->SetShader(_shader.get());
	device->SetConstantData(&_spriteInfo._worldMatrix, sizeof(_spriteInfo._worldMatrix));
	_constantBuffer->Update(device, &_spriteInfo._worldMatrix, sizeof(_spriteInfo._worldMatrix)); // @todo: move this to be per frame
	device->SetConstantBuffer(_constantBuffer.get());
	device->DrawTriangles(_vertexBuffer.get());
}