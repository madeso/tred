#pragma once

namespace render
{

struct Engine;

// "global" data on how many lights are supported
struct LightParams
{
    int number_of_directional_lights;
    int number_of_point_lights;
    int number_of_spot_lights;
};

const LightParams& get_light_params(const Engine& engine);

}

