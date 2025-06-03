#pragma once

#include <memory>

#include "Core/ApiExport.hpp"
#include "Math/Math.hpp"

class IRenderDevice;
class IConstantBuffer;

class ICamera
{
public:
	LIBRARY_API virtual void Select(IRenderDevice* device) = 0;
	LIBRARY_API virtual void SetRenderInfo(IRenderDevice* device) = 0;
};

class OrthographicCamera : public ICamera
{
public:
	LIBRARY_API OrthographicCamera(IRenderDevice* device);
	LIBRARY_API ~OrthographicCamera();

	LIBRARY_API void SetInfo(SkFloat2 position, SkFloat2 size);

	LIBRARY_API void Select(IRenderDevice* device) override;
	LIBRARY_API void SetRenderInfo(IRenderDevice* device) override;

private:
	struct ConstantBuffer
	{
		SkMatrix _matrix;
	};
	ConstantBuffer _orthographicVP;

	std::unique_ptr<IConstantBuffer> _constantBuffer;
};