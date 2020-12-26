#include "tred/input-commondef.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include "fmt/format.h"

#include "tred/input-config.h"

#include "tred/input-actionmap.h"
#include "tred/input-bindmap.h"
#include "tred/input-action.h"


namespace input
{

void TransformAndSetBindValue(const BindData& data, float value)
{
    float transformed = value * data.scale;
    
    if (data.invert)
    {
        transformed *= -1;
    }

    data.bind->value = transformed;
}


}  // namespace input
