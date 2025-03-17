#include "DirectX12Texture.hpp"

#include <cassert>

#include <png.h>
#include <zlib.h>

#include "File/File.hpp"
#include "Render/DirectX12/DirectX12Device.hpp"

void user_read_data(png_structp png_ptr, png_bytep data, size_t length)
{
	voidp read_io_ptr = png_get_io_ptr(png_ptr);
	IFile* file = reinterpret_cast<IFile*>(read_io_ptr);
	file->Read(data, length);
}

DirectX12Texture::DirectX12Texture(IRenderDevice* device, IFile* file)
{
	DirectX12Device* dx12Device = static_cast<DirectX12Device*>(device);
	if (file->IsOpen())
	{
		// only support loading in PNG files for now
		// @TODO: Move this to a shared area (make texture just take in raw data to make texture from?)

		png_structp pngReadStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		png_infop pngInfo = png_create_info_struct(pngReadStruct);

		png_set_read_fn(pngReadStruct, file, &user_read_data);

		// Ideally, we probably want to check the header and make sure it is something we support, but for now just read the whole thing
		png_read_png(pngReadStruct, pngInfo, PNG_TRANSFORM_IDENTITY, nullptr);

		// @TODO: Need to determine if we need to create textures with sRGB format (assuming PNG file is encoded with sRGB by checking gamma info)

		uint32_t width;
		uint32_t height;
		int32_t bitDepth;
		int32_t colorType;
		int32_t interlaceType;
		int32_t compressionMethod;
		int32_t filterMethod;
		png_get_IHDR(pngReadStruct, pngInfo, &width, &height, &bitDepth, &colorType, &interlaceType, &compressionMethod, &filterMethod);

		// handle if we have a palette
		png_colorp pngPalette = nullptr;
		int32_t numPalette = 0;
		int32_t numChannels = 0;
		png_bytep transAlpha = nullptr; // alpha values for palette images
		int32_t numTransAlpha = 0;
		png_color_16p transColors = nullptr; // set to single transparent color if not paletted
		png_get_tRNS(pngReadStruct, pngInfo, &transAlpha, &numTransAlpha, &transColors);
		if (colorType & PNG_COLOR_TYPE_PALETTE)
		{
			png_get_PLTE(pngReadStruct, pngInfo, &pngPalette, &numPalette);
			numChannels = 1;
		}
		else if (colorType & PNG_COLOR_TYPE_RGB)
		{
			assert(false); // not yet implemented
			numChannels = 3;
		}
		else if (colorType & PNG_COLOR_TYPE_RGBA)
		{
			assert(false); // not yet implemented
			numChannels = 4;
		}

		// just allocate RGBA8 data for all our textures for now
		constexpr uint32_t TEXTURE_PIXEL_SIZE = 4;
		std::vector<uint8_t> textureData(width * height * TEXTURE_PIXEL_SIZE);

		png_bytepp pngData = png_get_rows(pngReadStruct, pngInfo);
		for (uint32_t y = 0; y < height; y++)
		{
			for (uint32_t x = 0; x < width; x++)
			{
				uint8_t* pngPixel = &pngData[y][x * numChannels];
				uint8_t* texPixel = &textureData[(y * width * TEXTURE_PIXEL_SIZE) + (x * TEXTURE_PIXEL_SIZE)];
				if (numPalette > 0)
				{
					png_color color = pngPalette[*pngPixel];
					png_byte alpha = 0xff;
					if (transAlpha)
					{
						alpha = transAlpha[*pngPixel];
					}
					texPixel[0] = color.red;
					texPixel[1] = color.green;
					texPixel[2] = color.blue;
					texPixel[3] = alpha;
				}
				else
				{
					// handle RGB/RGBA here
					assert(false); // not yet implemented
				}
			}
		}

		png_destroy_read_struct(&pngReadStruct, &pngInfo, nullptr);

		D3D12_HEAP_PROPERTIES heapProperties = {
			.Type = D3D12_HEAP_TYPE_DEFAULT,
			.CreationNodeMask = 1,
			.VisibleNodeMask = 1,
		};

		D3D12_RESOURCE_DESC textureDesc = {
			.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
			.Alignment = 0,
			.Width = width,
			.Height = height,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = D3D12_RESOURCE_FLAG_NONE,
		};

		dx12Device->GetDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&textureDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&_texture)
		);

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT subresourceFootprint;
		D3D12_RESOURCE_DESC footprintTextureDesc = _texture->GetDesc();
		uint32_t numRows = 0;
		uint64_t rowSizeInBytes = 0;
		uint64_t requiredSize = 0;
		dx12Device->GetDevice()->GetCopyableFootprints(&footprintTextureDesc, 0, 1, 0, &subresourceFootprint, &numRows, &rowSizeInBytes, &requiredSize);

		D3D12_RESOURCE_DESC resourceDesc = {
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Width = requiredSize,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.SampleDesc = {
				.Count = 1
			},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		};
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		dx12Device->GetDevice()->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&_uploadTexture)
		);

		D3D12_RANGE range = {
			.Begin = 0,
			.End = 0,
		};
		uint8_t* uploadData = nullptr;
		_uploadTexture->Map(0, &range, reinterpret_cast<void**>(&uploadData));
		for (uint32_t y = 0; y < height; y++)
		{
			memcpy(&uploadData[y * subresourceFootprint.Footprint.RowPitch], &textureData[y * width * TEXTURE_PIXEL_SIZE], width* TEXTURE_PIXEL_SIZE);
		}
		_uploadTexture->Unmap(0, nullptr);

		D3D12_TEXTURE_COPY_LOCATION dest = {
			.pResource = _texture.Get(),
			.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX
		};
		D3D12_TEXTURE_COPY_LOCATION src = {
			.pResource = _uploadTexture.Get(),
			.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
			.PlacedFootprint = subresourceFootprint,
		};
		dx12Device->GetCommandList()->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);
	
		D3D12_RESOURCE_BARRIER barrier = {
			.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			.Transition = {
				.pResource = _texture.Get(),
				.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
				.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST,
				.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			},
		};
		dx12Device->GetCommandList()->ResourceBarrier(1, &barrier);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
			.Format = textureDesc.Format,
			.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
			.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
			.Texture2D = {
				.MipLevels = 1,
			},
		};
		dx12Device->GetDevice()->CreateShaderResourceView(_texture.Get(), &srvDesc, dx12Device->GetSRVHeap()->GetCPUDescriptorHandleForHeapStart());
	}
}

DirectX12Texture::~DirectX12Texture()
{
}
