#pragma once

#include "tred/render/handle.mesh.h"
#include "tred/render/light.active.h"

struct Camera;
struct DirectionalLight;
struct PointLight;
struct SpotLight;


namespace render
{

struct RenderCommand
{
    GeomId geom_id;
    MaterialId material_id;
    glm::mat4 model;
};
// bool operator<(const RenderCommand& lhs, const RenderCommand& rhs) { return lhs.id < rhs.id; }


// also called render queue but as a non-native speaker I hate typing queue
struct RenderList
{
    // todo(Gustav): sort commands!
    // std::priority_queue<RenderCommand> commands;
    // todo(Gustav): test perf with a std::multiset or a std::vector (sorted in render)
    std::vector<RenderCommand> commands;

    LightData lights;
    glm::vec3 camera_position;
    glm::mat4 projection_view;
    LightStatus light_status;
    std::optional<MaterialId> global_shader;

    bool is_rendering = false;

    void begin_perspective(float aspect_ratio, const Camera& camera, std::optional<MaterialId> the_global_shader);
    void end();

    void add_directional_light(const DirectionalLight& d);
    void add_point_light(const PointLight& p);
    void add_spot_light(const SpotLight& s);
    void add_mesh(GeomId geom_id, MaterialId material_id, const glm::mat4& model);

};

}

