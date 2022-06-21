#include "tred/render/shader.template.h"

#include "mustache/mustache.hpp"

namespace render
{

std::string
generate(std::string_view str, const ShaderCompilerProperties& properties)
{
    auto input = kainjow::mustache::mustache{std::string{str.begin(), str.end()}};
    input.set_custom_escape([](const std::string& s) { return s; });
    auto data = kainjow::mustache::data{};
    for(const auto& kv: properties)
    {
        data[kv.first] = kv.second;
    }
    return input.render(data);
}

}
