#include "tred/render/light.active.h"

namespace render
{


void LightData::clear()
{
    directional_lights.clear();
    point_lights.clear();
    spot_lights.clear();
}


}

