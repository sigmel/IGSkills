#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Math/Math.hpp"

class IRenderDevice;

class IVertexBuffer
{
public:
    static const std::string kPosition;
    static const std::string kColor;
    static const std::string kUV;
    static const std::string kNormal;

    struct VertexPosColor
    {
        SkFloat3 _position;
        SkFloat4 _color;

        static const std::vector<std::string> kDescription;
    };

    struct VertexPosUV
    {
        SkFloat3 _position;
        SkFloat2 _uv;

        static const std::vector<std::string> kDescription;
    };

    struct VertexPosNormalColor
    {
        SkFloat3 _position;
        SkFloat3 _normal;
        SkFloat4 _color;

        static const std::vector<std::string> kDescription;
    };

    static std::unique_ptr<IVertexBuffer> Make(IRenderDevice* device, const void* data, size_t dataSize, size_t stride);
};