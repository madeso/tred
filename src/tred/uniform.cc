#include "tred/uniform.h"


uniform::uniform()
    : name("<unknown>")
    , location(-1)
    , debug_shader_program(0)
{
}


uniform::uniform(const std::string& n, int l, unsigned int sp)
    : name(n)
    , location(l)
    , debug_shader_program(sp)
{
}


uniform::operator bool() const
{
    return is_valid();
}


bool
uniform::is_valid() const
{
    return location >= 0;
}

