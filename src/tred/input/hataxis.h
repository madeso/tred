#pragma once

#include "tred/input/axis.h"


namespace input
{


struct HatAndAxis2
{
    HatAndAxis2(int hat, Axis2 axis);

    bool operator==(const HatAndAxis2& rhs) const;

    int hat;
    Axis2 axis;
};

}



namespace std
{
    template <>
    struct hash<input::HatAndAxis2>
    {
        std::size_t operator()(const input::HatAndAxis2& k) const
        {
            using std::size_t;
            using std::hash;
            using std::string;

            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:

            return ((hash<int>()(k.hat)
                ^ (hash<input::Axis2>()(k.axis) << 1)) >> 1)
                ;
        }
    };
}



