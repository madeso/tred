#pragma once

namespace render
{

enum class PropertyType
{
    float_type, vec3_type, vec4_type, texture_type
};

struct PropertyIndex
{
    PropertyType type;
    int index;
};

bool operator<(const PropertyIndex& lhs, const PropertyIndex& rhs);

}

