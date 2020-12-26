#include "tred/input-table.h"

void Table::Set(const std::string& name, float value)
{
    data[name] = value;
}
