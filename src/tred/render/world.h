#pragma once

#include <memory>

#include "tred/render/handles.h"


namespace render
{

struct DirectionalLight;
struct PointLight;
struct SpotLight;
struct RenderList;

enum class ActorId : u64 {};
enum class DirectionalLightId : u64 {};
enum class SpotLightId : u64 {};
enum class PointLightId : u64 {};

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
    virtual void render(RenderList* renderer) = 0;
};

std::unique_ptr<World> create_null_world();

}
