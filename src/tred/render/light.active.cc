#include "tred/render/light.active.h"

namespace render
{


void LightData::clear()
{
    directional_lights.clear();
    point_lights.clear();
    spot_lights.clear();
}


LightStatus LightStatus::create_no_error()
{
    return {true, true, true};
}


}

