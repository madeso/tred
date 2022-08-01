#include "tred/render/engine.h"

#include "tred/assert.h"
#include "tred/handle.h"
#include "tred/log.h"

#include "tred/render/camera.h"
#include "tred/render/compiled.mesh.h"
#include "tred/render/handle.funcs.h"
#include "tred/render/handles.h"
#include "tred/render/light.h"
#include "tred/render/render_func.h"
#include "tred/render/render_world_interface.h"
#include "tred/render/texture.h"
#include "tred/render/vfs.h"
#include "tred/render/world.h"
#include "tred/render/render2.h"


namespace render
{

struct RenderListCommand
{
    GeomId geom_id;
    MaterialId material_id;
    glm::mat4 model;
};
// bool operator<(const RenderListCommand& lhs, const RenderListCommand& rhs) { return lhs.id < rhs.id; }


// also called render queue but as a non-native speaker I hate typing queue
struct RenderList
{
    // todo(Gustav): sort commands!
    // std::priority_queue<RenderListCommand> commands;
    // todo(Gustav): test perf with a std::multiset or a std::vector (sorted in render)
    std::vector<RenderListCommand> commands;
    ActiveLights lights;

    std::optional<MaterialId> global_shader;

    bool is_rendering = false;
};

struct Cache
{
    HandleVector64<Texture, TextureId> textures;
    HandleVector64<CompiledMaterialShader, CompiledMaterialShaderId> shaders;

    std::optional<TextureId> default_texture;
};

struct EngineData
{
    Vfs* vfs;
    LightParams light_params;
    Cache cache;
    HandleVector64<CompiledGeom, GeomId> geoms;
    HandleVector64<CompiledMaterial, MaterialId> materials;
    std::vector<VertexType> global_layout;
    bool added_meshes = false;
    RenderList render;

    EngineData(Vfs* a_vfs, const LightParams& alp);
};


void
sr_add_directional_light(RenderList* rl,const DirectionalLight& d)
{
    ASSERT(rl->is_rendering);
    rl->lights.directional_lights.emplace_back(d);
}


void
sr_add_point_light(RenderList* rl,const PointLight& p)
{
    ASSERT(rl->is_rendering);
    rl->lights.point_lights.emplace_back(p);
}


void
sr_add_spot_light(RenderList* rl,const SpotLight& s)
{
    ASSERT(rl->is_rendering);
    rl->lights.spot_lights.emplace_back(s);
}


void
sr_add_mesh(RenderList* rl, GeomId geom_id, MaterialId material_id, const glm::mat4& model)
{
    ASSERT(rl->is_rendering);
    rl->commands.emplace_back(RenderListCommand{geom_id, rl->global_shader.value_or(material_id), model});
}







#define ADD_OP(FUNC_NAME, MEMBER, TYPE, ENUM)\
void set_##FUNC_NAME##_by_lookup\
(\
    CompiledMaterial* mat,\
    const EngineData& data,\
    const HashedString& name,\
    const TYPE& v\
)\
{\
    const auto& cms = get_compiled_material_shader(data.cache, mat->shader);\
    const auto found = cms.name_to_array.find(name);\
    ASSERT(found != cms.name_to_array.end());\
    mat->properties.set_##FUNC_NAME(found->second, v);\
}

ADD_OP(float, floats, float, MaterialPropertyType::float_type)
ADD_OP(vec3, vec3s, glm::vec3, MaterialPropertyType::vec3_type)
ADD_OP(vec4, vec4s, glm::vec4, MaterialPropertyType::vec4_type)
ADD_OP(texture, textures, TextureId, MaterialPropertyType::texture_type)
#undef ADD_OP







const Texture& get_texture(const Cache& cache, TextureId id)
{
    return cache.textures[id];
}

TextureId load_texture(Cache* cache, const Vfs& vfs, const std::string& path)
{
    auto texture = vfs.load_texture(path);
    if(texture)
    {
        return cache->textures.add(std::move(*texture));
    }

    // todo(Gustav): construct default texture with path? based on normal?
    LOG_WARNING("Unable to load texture {}", path);
    // todo(Gustav): add default texture
    return *cache->default_texture;
}


CompiledMaterialShaderId
load_compiled_material_shader
(
    Engine* engine,
    const Vfs& vfs,
    Cache* cache,
    const std::string& path
)
{
    auto shader = load_material_shader
    (
        engine->data->global_layout,
        cache,
        vfs,
        path,
        engine->data->light_params
    );
    if(shader)
    {
        return cache->shaders.add(std::move(*shader));
    }

    // todo(Gustav): load default shader?
    LOG_ERROR("Unable to load shader {}", path);
    DIE("unable to load shader");
    return static_cast<CompiledMaterialShaderId>(0);
}


const CompiledMaterialShader& get_compiled_material_shader(const Cache& cache, CompiledMaterialShaderId id)
{
    return cache.shaders[id];
}



EngineData::EngineData(Vfs* a_vfs, const LightParams& alp)
    : vfs(a_vfs)
    , light_params(alp)
{
}

Engine::Engine
(
    Vfs* vfs,
    int number_of_directional_lights,
    int number_of_point_lights,
    int number_of_spot_lights
)
    : data
    (
        std::make_unique<EngineData>
        (
            vfs,
            LightParams
                {
                    number_of_directional_lights,
                    number_of_point_lights,
                    number_of_spot_lights
                }
        )
    )
{
}

Engine::~Engine() = default;

AddedMesh Engine::add_mesh(const Mesh& mesh)
{
    data->added_meshes = true;
    auto compiled = compile_mesh(this, &data->cache, *data->vfs, mesh);
    return {data->geoms.add(std::move(compiled.geom)), data->materials.add(std::move(compiled.material))};
}

MaterialId Engine::add_global_shader(const MaterialDescription& path)
{
    ASSERT(data->added_meshes == false);
    auto material = compile_material(this, &data->cache, *data->vfs, path);
    const auto& shader = get_compiled_material_shader(data->cache, material.shader);
    data->global_layout = [&]() // todo(Gustav): replace with range usage
    {
        std::vector<VertexType> r;
        for(const auto& e: shader.mesh_layout.elements)
        {
            r.emplace_back(e.type);
        }
        return r;
    }();

    return data->materials.add(std::move(material));
}

MaterialId Engine::duplicate_material(MaterialId id)
{
    CompiledMaterial copy = data->materials[id];
    return data->materials.add(std::move(copy));
}

CompiledMaterial& Engine::get_material_ref(MaterialId id)
{
    return data->materials[id];
}




void
render_world
(
    Engine* engine,
    const RenderCommand& rc,
    World& world,
    float aspect_ratio,
    const Camera& camera,
    std::optional<MaterialId> global_shader
)
{
    RenderList* rl = &engine->data->render;

    // prepare rendering ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    rl->global_shader = global_shader;
    rl->commands.clear();
    rl->lights.clear();
    const auto projection_view = [&]
    {
        const glm::mat4 projection = glm::perspective
        (
            glm::radians(camera.fov),
            aspect_ratio,
            camera.near, camera.far
        );
        const auto camera_space = create_vectors(camera);
        const auto view = glm::lookAt
        (
            camera.position,
            camera.position + camera_space.front,
            UP
        );
        return projection * view;
    }();

    // add items to list ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    rl->is_rendering = true;
    world.render(rl);
    rl->is_rendering = false;

    // process render list ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    LightStatus light_status = k_lightstatus_without_errors;
    clear_stencil_and_depth();
    for(const auto& c: rl->commands)
    {
        use_material
        (
            engine->data->materials[c.material_id],
            rc.states,
            engine->data->cache,
            {
                camera.position,
                projection_view,
                c.model
            },
            rl->lights,
            &light_status
        );
        render_geom(engine->data->geoms[c.geom_id]);
    }
}

}
