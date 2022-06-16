#include "tred/render/renderlist.h"

#include "tred/assert.h"

#include "tred/render/camera.h"
#include "tred/render/camera.compiled.h"
#include "tred/render/light.h"


namespace render
{



void RenderList::begin_perspective(float aspect_ratio, const Camera& camera, std::optional<MaterialId> the_global_shader)
{
    ASSERT(is_rendering == false);
    is_rendering = true;

    const glm::mat4 projection = glm::perspective(glm::radians(camera.fov), aspect_ratio, camera.near, camera.far);
    const auto compiled_camera = compile_camera(camera.create_vectors());
    const auto view = compiled_camera.view;
    const auto pv = projection * view;

    camera_position = compiled_camera.position;
    projection_view = pv;
    global_shader = the_global_shader;

    light_status = LightStatus::create_no_error();
    commands.clear();
    lights.clear();
}

void RenderList::add_directional_light(const DirectionalLight& d)
{
    ASSERT(is_rendering);
    lights.directional_lights.emplace_back(d);
}

void RenderList::add_point_light(const PointLight& p)
{
    ASSERT(is_rendering);
    lights.point_lights.emplace_back(p);
}

void RenderList::add_spot_light(const SpotLight& s)
{
    ASSERT(is_rendering);
    lights.spot_lights.emplace_back(s);
}

void RenderList::add_mesh(GeomId geom_id, MaterialId material_id, const glm::mat4& model)
{
    ASSERT(is_rendering);
    commands.emplace_back(RenderCommand{geom_id, global_shader.value_or(material_id), model});
}

void RenderList::end()
{
    ASSERT(is_rendering);
    is_rendering = false;
}


}
