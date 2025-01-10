#include "File/File.hpp"

#include <fstream>
#include <filesystem>

using namespace std;

// This will represent your standard file
// just using basic IO stream for now, but could extend this to more efficient
// ways in the future if I really need/want to
class OSFile final : public IFile
{
public:
	OSFile(u16string path)
		: _path(path),
		_stream(_path, ios::in | ios::binary)
	{
	}

	virtual ~OSFile() {}

	bool IsOpen() override
	{
		return _stream.is_open();
	}

	void Read(uint8_t* data, size_t length) override
	{
		_stream.read(reinterpret_cast<char*>(data), length);
	}

	void Load(vector<uint8_t>& data) override
	{
		_stream.seekg(0, ios::end);
		streampos fileSize = _stream.tellg();
		data.resize(fileSize);
		_stream.seekg(0, ios::beg);

		_stream.read(reinterpret_cast<char*>(&data[0]), fileSize);
	}

private:
	filesystem::path _path;
	fstream _stream;
};

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

unique_ptr<IFile> FileManager::OpenFile(u16string filePath)
{
	return make_unique<OSFile>(filePath);
}