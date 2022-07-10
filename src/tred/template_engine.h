#pragma once

#include <unordered_map>

namespace template_engine
{

using Properties = std::unordered_map<std::string, std::string>;

std::string
generate(std::string_view str, const Properties& properties);

}
