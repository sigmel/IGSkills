#include "Buffer.hpp"

#if RENDERAPI_DIRECTX12
#include "DirectX12/DirectX12Buffer.hpp"
#else
#error "No valid Render API defined"
#endif

const std::string IVertexBuffer::kPosition{ "POSITION" };
const std::string IVertexBuffer::kColor{ "COLOR" };
const std::string IVertexBuffer::kUV{ "UV" };
const std::string IVertexBuffer::kNormal{ "NORMAL" };

const std::vector<std::string> IVertexBuffer::VertexPosColor::kDescription = {
	kPosition,
	kColor
};

const std::vector<std::string> IVertexBuffer::VertexPosUV::kDescription = {
	kPosition,
	kUV
};

const std::vector<std::string> IVertexBuffer::VertexPosNormalColor::kDescription = {
	kPosition,
	kNormal,
	kColor
};

std::unique_ptr<IVertexBuffer> IVertexBuffer::Make(IRenderDevice* device, const void* data, size_t dataSize, size_t stride)
{
#if RENDERAPI_DIRECTX12
	return std::make_unique<DirectX12VertexBuffer>(device, data, dataSize, stride);
#else
	return nullptr;
#endif
}