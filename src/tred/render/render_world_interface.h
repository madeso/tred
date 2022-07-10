#pragma once

#include "tred/render/handles.h"


namespace render
{

struct DirectionalLight;
struct PointLight;
struct SpotLight;
struct Camera;
struct EngineData;

struct RenderList;

void sr_add_directional_light(RenderList* sr, const DirectionalLight& d);
void sr_add_point_light(RenderList* sr, const PointLight& p);
void sr_add_spot_light(RenderList* sr, const SpotLight& s);
void sr_add_mesh(RenderList* sr, GeomId geom_id, MaterialId material_id, const glm::mat4& model);

}
