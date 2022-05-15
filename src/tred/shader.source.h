#pragma once

#include <optional>
#include <vector>
#include <string_view>
#include <string>

#include "vertex_layout.h"

namespace shader
{
enum class ShaderMessageType
{
    info, warning, error
};

struct ShaderMessage
{
    ShaderMessageType type;
    int line;
    std::string message;
};

struct ShaderSource
{
    ShaderVertexAttributes layout;
    std::string vertex;
    std::string fragment;
};

using ShaderLog = std::vector<ShaderMessage>;

struct ShaderResult
{
    std::optional<ShaderSource> source;
    ShaderLog log;
};


ShaderResult parse_shader_source(std::string_view source);
}
