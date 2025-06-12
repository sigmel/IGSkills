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

inline SkMatrix SkAffineMatrix(SkFloat3 translation, SkFloat3 scale)
{
	return SkMatrix(scale.x, 0.0f, 0.0f, 0.0f,
					0.0f, scale.y, 0.0f, 0.0f,
					0.0f, 0.0f, scale.z, 0.0f,
					translation.x, translation.y, translation.z, 1.0f);
}

inline constexpr size_t SkAlignPow2(size_t size, size_t align)
{
	return (size + (align - 1)) & ~(align - 1);
}