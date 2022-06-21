#pragma once

#include <map>

namespace render
{

using ShaderCompilerProperties = std::map<std::string, std::string>;

std::string
generate(std::string_view str, const ShaderCompilerProperties& properties);

}
