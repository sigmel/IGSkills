#pragma once

#include <memory>

class IRenderDevice;
class IFile;

class ITexture
{
public:
	virtual ~ITexture() {}

	static std::unique_ptr<ITexture> Make(IRenderDevice* device, IFile* file);
};