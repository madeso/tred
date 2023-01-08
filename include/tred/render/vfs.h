#pragma once


namespace render
{

struct Texture;
struct MaterialShaderSource;


struct Vfs
{
    Vfs() = default;
    virtual ~Vfs() = default;

    Vfs(const Vfs&) = delete;
    Vfs(Vfs&&) = delete;
    
    void operator=(const Vfs&) = delete;
    void operator=(Vfs&&) = delete;

    virtual std::optional<MaterialShaderSource> load_material_shader_source(const std::string& path) const = 0;
    virtual std::optional<Texture> load_texture(const std::string& path) const = 0;
};

}
