#include "tred/input/table.h"


namespace input
{


void Table::Set(const std::string& name, float value)
{
    data[name] = value;
}


}

