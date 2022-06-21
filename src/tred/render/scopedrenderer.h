#pragma once

#include "tred/render/handle.mesh.h"


namespace render
{

struct DirectionalLight;
struct PointLight;
struct SpotLight;
struct Camera;
struct Engine;


enum class ScopedRendererState
{
    adding, complete
};


struct ScopedRenderer
{
    Engine* engine;
    ScopedRendererState render_state = ScopedRendererState::adding;

    ScopedRenderer() = default;
    
    ScopedRenderer(const ScopedRenderer&) = delete;
    ScopedRenderer(ScopedRenderer&&) = delete;
    void operator=(const ScopedRenderer&) = delete;
    void operator=(ScopedRenderer&&) = delete;

    void add_directional_light(const DirectionalLight& d);
    void add_point_light(const PointLight& p);
    void add_spot_light(const SpotLight& s);
    void add_mesh(GeomId geom_id, MaterialId material_id, const glm::mat4& model);
    void render_all();

    ~ScopedRenderer();
};

ScopedRenderer create_render_list_for_perspective(Engine* engine, float aspect_ratio, const Camera& camera, std::optional<MaterialId> global_shader);

}
