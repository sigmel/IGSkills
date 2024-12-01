#pragma once

#include <memory>
#include <string>
#include <vector>

class IRenderDevice;

class IShader
{
public:
	static std::unique_ptr<IShader> Make(IRenderDevice* device, const std::wstring& filename, const std::string& vertexFunction, const std::string& pixelFunction, const std::vector<std::string>& vertexDescription);
};