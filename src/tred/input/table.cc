#include "tred/input/table.h"


namespace input
{


void table::set(const std::string& name, float value)
{
    data[name] = value;
}


}

