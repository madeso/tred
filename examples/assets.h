#pragma once

#include "tred/hash.string.h"
#include "tred/render/vfs.h"

constexpr auto diffuse_color = HashedStringView{"Diffuse color"};
constexpr auto diffuse_texture = HashedStringView{"Diffuse texture"};
constexpr auto specular_texture = HashedStringView{"Specular texture"};
constexpr auto shininess_prop = HashedStringView{"Shininess"};
constexpr auto specular_strength_prop = HashedStringView{"Specular strength"};
constexpr auto tint_prop = HashedStringView{"Tint color"};

namespace assets
{

struct FixedFileVfs : render::Vfs
{
    std::optional<render::MaterialShaderSource> load_material_shader_source(const std::string& path) const override;
    std::optional<Texture> load_texture(const std::string& path) const override;
};

}
