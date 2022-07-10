#include "tred/render/world.h"

#include "tred/handle.h"
#include "tred/render/light.h"
#include "tred/render/render_world_interface.h"

namespace render
{

struct Actor
{
    GeomId geom;
    MaterialId material;

    // todo(Gustav): change to a representation that is easier to update and cull and can easily build a model matrix?
    // float yaw, pitch, roll; float x, y, z; float scale_uniform;
    glm::mat4 transform;
};


struct NullWorld : World
{
    NullWorld() = default;
    virtual ~NullWorld() = default;

    NullWorld(const NullWorld&) = delete;
    NullWorld(NullWorld&&) = delete;
    void operator=(const NullWorld&) = delete;
    void operator=(NullWorld&&) = delete;

    HandleVector64<Actor, ActorId> actors;

    HandleVector64<DirectionalLight, DirectionalLightId> directional_lights;
    HandleVector64<SpotLight, SpotLightId> spot_lights;
    HandleVector64<PointLight, PointLightId> point_lights;

    ActorId add_actor(GeomId geom_id, MaterialId material_id, const glm::mat4& transform)
    {
        return actors.add(Actor{geom_id, material_id, transform});
    }

    void update_actor(ActorId id, const glm::mat4& transform)
    {
        actors[id].transform = transform;
    }

    DirectionalLightId add_directional_light(const DirectionalLight& light)
    {
        return directional_lights.add(DirectionalLight{light});
    }

    void update_directional_light(DirectionalLightId id, const DirectionalLight& light)
    {
        directional_lights[id] = light;
    }

    SpotLightId add_spot_light(const SpotLight& light)
    {
        return spot_lights.add(SpotLight{light});
    }

    void update_spot_light(SpotLightId id, const SpotLight& light)
    {
        spot_lights[id] = light;
    }

    PointLightId add_point_light(const PointLight& light)
    {
        return point_lights.add(PointLight{light});
    }

    void update_point_light(PointLightId id, const PointLight& light)
    {
        point_lights[id] = light;
    }

    void render(RenderList* renderer)
    {
        for(const auto& light: directional_lights) { sr_add_directional_light(renderer, light); }
        for(const auto& light: spot_lights) { sr_add_spot_light(renderer, light); }
        for(const auto& light: point_lights) { sr_add_point_light(renderer, light); }
        for(const auto& act: actors) { sr_add_mesh(renderer, act.geom, act.material, act.transform); }
    }
};


std::unique_ptr<World> create_null_world()
{
    return std::make_unique<NullWorld>();
}

}

