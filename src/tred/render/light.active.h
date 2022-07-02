#pragma once

#include "tred/render/light.h"


namespace render
{


struct LightData
{
    std::vector<DirectionalLight> directional_lights;
    std::vector<PointLight> point_lights;
    std::vector<SpotLight> spot_lights;

    void clear();
};

struct LightStatus
{
    bool applied_directional_lights;
    bool applied_point_lights;
    bool applied_spot_lights;
};

constexpr LightStatus k_lightstatus_without_errors{true, true, true};

}
