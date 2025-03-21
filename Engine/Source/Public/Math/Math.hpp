#pragma once

#include "DirectXMath.h"

typedef DirectX::XMFLOAT2 SkFloat2;
typedef DirectX::XMFLOAT3 SkFloat3;
typedef DirectX::XMFLOAT4 SkFloat4;
typedef DirectX::XMMATRIX SkMatrix;

inline SkMatrix SkIdentityMatrix()
{
	return SkMatrix(1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
}

inline constexpr size_t SkAlignPow2(size_t size, size_t align)
{
	return (size + (align - 1)) & ~(align - 1);
}