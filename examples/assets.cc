#include "assets.h"

#include "tred/colors.h"
#include "tred/log.h"

#include "tred/render/shader.h"
#include "tred/render/material.shader.source.h"
#include "tred/render/texture.h"
#include "tred/render/texture.load.h"

#include "shader_light.glsl.h"
#include "shader_material.glsl.h"
#include "container_diffuse.png.h"
#include "container_specular.png.h"


namespace assets
{

void log_shader_error(const std::string& file, const shader::ShaderResult& res)
{
    for(const auto& e: res.log)
    {
        switch(e.type)
        {
        case shader::ShaderMessageType::info:
            LOG_INFO("{}({}): {}", file, e.line, e.message);
            break;
        case shader::ShaderMessageType::warning:
            LOG_WARNING("{}({}): {}", file, e.line, e.message);
            break;
        case shader::ShaderMessageType::error:
            LOG_ERROR("{}({}): {}", file, e.line, e.message);
            break;
        default:
            LOG_ERROR("INVALID LOG ENTRY {}({}): {}", file, e.line, e.message);
            break;
        }
    }
}


std::optional<render::MaterialShaderSource> FixedFileVfs::load_material_shader_source(const std::string& path) const
{
    if(path == "default.glsl")
    {
        const auto src = shader::parse_shader_source(SHADER_MATERIAL_GLSL);
        log_shader_error(path, src);
        if(src.source.has_value() == false) { LOG_ERROR("Failed to parse shader file {}", path); return std::nullopt; }
        return render::MaterialShaderSource::create_with_lights(*src.source)
            .with_texture(diffuse_texture, "uMaterial.diffuse", "white.png")
            .with_texture(specular_texture, "uMaterial.specular", "no-specular.png")
            .with_vec4(diffuse_color, "uColor", glm::vec4{white3, 1.0f})
            .with_vec3(tint_prop, "uMaterial.tint", white3)
            .with_float(shininess_prop, "uMaterial.shininess", 32.0f)
            .with_float(specular_strength_prop, "uMaterial.specular_strength", 1.0f)
            ;
    }
    if(path == "unlit.glsl")
    {
        const auto src = shader::parse_shader_source(SHADER_LIGHT_GLSL);
        log_shader_error(path, src);
        if(src.source.has_value() == false) { LOG_ERROR("Failed to parse shader file {}", path); return std::nullopt; }
        return render::MaterialShaderSource::create_unlit(*src.source)
            .with_vec3(diffuse_color, "uColor", glm::vec3{1.0f, 1.0f, 1.0f})
            ;
    }
    return std::nullopt;
}

std::optional<Texture> FixedFileVfs::load_texture(const std::string& path) const
{
    if(path == "container_diffuse.png")
    {
        return load_image_from_embedded
        (
            CONTAINER_DIFFUSE_PNG,
            TextureEdge::repeat,
            TextureRenderStyle::smooth,
            Transparency::exclude
        );
    }
    if(path == "container_specular.png")
    {
        return load_image_from_embedded
        (
            CONTAINER_SPECULAR_PNG,
            TextureEdge::repeat,
            TextureRenderStyle::smooth,
            Transparency::exclude
        );
    }
    if(path == "white.png")
    {
        return load_image_from_color
        (
            0xFFFFFFFF,
            TextureEdge::repeat,
            TextureRenderStyle::smooth,
            Transparency::exclude
        );
    }
    if(path == "no-specular.png")
    {
        return load_image_from_color
        (
            0x000000FF,
            TextureEdge::repeat,
            TextureRenderStyle::smooth,
            Transparency::exclude
        );
    }
    return std::nullopt;
}

}
