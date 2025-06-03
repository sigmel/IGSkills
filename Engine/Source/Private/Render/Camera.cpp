#include "Render/Camera.hpp"

#include "Render/RenderDevice.hpp"
#include "Render/Buffer.hpp"

OrthographicCamera::OrthographicCamera(IRenderDevice* device):
	_constantBuffer(IConstantBuffer::Make(device, sizeof(ConstantBuffer)))
{
	_orthographicVP._matrix = SkIdentityMatrix();
}

OrthographicCamera::~OrthographicCamera()
{
}

void OrthographicCamera::SetInfo(SkFloat2 position, SkFloat2 size)
{
	float l = position.x - (size.x / 2.0f);
	float r = position.x + (size.x / 2.0f);
	float t = position.y + (size.y / 2.0f);
	float b = position.y - (size.y / 2.0f);
	constexpr float n = -1.0f;
	constexpr float f = 1.0f;

	_orthographicVP._matrix = SkMatrix(
		2.0f / (r - l), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (t - b), 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f / (f - n), 0.0f,
		-(r + l) / (r - l), -(t + b) / (t - b), -(f + n) / (f - n), 1.0f
	);
}

void OrthographicCamera::Select(IRenderDevice* device)
{
	_constantBuffer->Update(device, &_orthographicVP._matrix, sizeof(_orthographicVP._matrix));
	device->SetCamera(this);
}

void OrthographicCamera::SetRenderInfo(IRenderDevice* device)
{
	device->SetConstantBuffer(_constantBuffer.get());
}