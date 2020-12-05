#include "tred/uniform.h"


Uniform::Uniform()
    : name("<unknown>")
    , location(-1)
    , debug_shader_program(0)
{
}


Uniform::Uniform(const std::string& n, int l, unsigned int sp)
    : name(n)
    , location(l)
    , debug_shader_program(sp)
{
}


Uniform::operator bool() const
{
    return IsValid();
}


bool
Uniform::IsValid() const
{
    return location >= 0;
}

