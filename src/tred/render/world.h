#pragma once

#include <memory>

#include "tred/render/handle.mesh.h"
#include "tred/render/handle.world.h"

struct DirectionalLight;
struct PointLight;
struct SpotLight;

namespace render
{

struct ScopedRenderer;

struct World
{
    World() = default;
    virtual ~World() = default;

    World(const World&) = delete;
    World(World&&) = delete;
    void operator=(const World&) = delete;
    void operator=(World&&) = delete;

    virtual ActorId add_actor(GeomId geom_id, MaterialId material_id, const glm::mat4& transform) = 0;
    virtual void update_actor(ActorId id, const glm::mat4& transform) = 0;
    virtual DirectionalLightId add_directional_light(const DirectionalLight& light) = 0;
    virtual void update_directional_light(DirectionalLightId id, const DirectionalLight& light) = 0;
    virtual SpotLightId add_spot_light(const SpotLight& light) = 0;
    virtual void update_spot_light(SpotLightId id, const SpotLight& light) = 0;
    virtual PointLightId add_point_light(const PointLight& light) = 0;
    virtual void update_point_light(PointLightId id, const PointLight& light) = 0;
    virtual void render(ScopedRenderer* renderer) = 0;
};

std::unique_ptr<World> create_null_world();

}
