#include "tred/input/table.h"


namespace input
{


void Table::set(const std::string& name, float value)
{
    data[name] = value;
}


}

