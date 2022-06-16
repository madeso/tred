#include "tred/render/scopedrenderer.h"

#include "tred/assert.h"

#include "tred/render/compiledmaterial.h"
#include "tred/render/compiled.mesh.h"
#include "tred/render/engine.h"
#include "tred/render_func.h"

namespace render
{


void render_geom_with_material
(
    const CompiledGeom& geom,
    const CompiledMaterial& material,
    const Cache& cache,
    const CommonData& data,
    const LightData& light_data,
    LightStatus* ls
)
{
    material.use(cache, data, light_data, ls);
    geom.draw();
}


void ScopedRenderer::add_directional_light(const DirectionalLight& d)
{
    ASSERT(render_state == ScopedRendererState::adding);
    engine->render.add_directional_light(d);
}

void ScopedRenderer::add_point_light(const PointLight& p)
{
    ASSERT(render_state == ScopedRendererState::adding);
    engine->render.add_point_light(p);
}

void ScopedRenderer::add_spot_light(const SpotLight& s)
{
    ASSERT(render_state == ScopedRendererState::adding);
    engine->render.add_spot_light(s);
}

void ScopedRenderer::add_mesh(GeomId geom_id, MaterialId material_id, const glm::mat4& model)
{
    ASSERT(render_state == ScopedRendererState::adding);
    engine->render.add_mesh(geom_id, material_id, model);
}

void ScopedRenderer::render_all()
{
    ASSERT(render_state == ScopedRendererState::adding);
    render_state = ScopedRendererState::complete;

    engine->render.end();

    clear_stencil_and_depth();
    for(const auto& c: engine->render.commands)
    {
        render_geom_with_material
        (
            engine->geoms[c.geom_id],
            engine->materials[c.material_id],
            engine->cache,
            {
                engine->render.camera_position,
                engine->render.projection_view,
                c.model
            },
            engine->render.lights,
            &engine->render.light_status
        );
    }
}

ScopedRenderer::~ScopedRenderer()
{
    ASSERT(render_state == ScopedRendererState::complete);
}

ScopedRenderer create_render_list_for_perspective(Engine* engine, float aspect_ratio, const Camera& camera, std::optional<MaterialId> global_shader)
{
    engine->render.begin_perspective(aspect_ratio, camera, global_shader);
    return {engine};
}

}
