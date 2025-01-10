#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Core/ApiExport.hpp"

// Making this an interface since there may be different file types in the future.
// Perhaps getting ahead of myself a little by doing this, but it'll be a simple interface to start.
class IFile
{
public:
	virtual ~IFile() {}

	virtual bool IsOpen() = 0;
	virtual void Read(uint8_t* data, size_t length) = 0;
	virtual void Load(std::vector<uint8_t>& data) = 0;
};

// We don't support any caching or the like yet, so this will return a file class that is the
// sole owner of that file and there can be multiple copies of it at once. Eventually, we could
// have a pool of files and the file class is more like a view into that file.
class FileManager
{
public:
	FileManager();
	~FileManager();

	std::unique_ptr<IFile> OpenFile(std::u16string filePath);
};