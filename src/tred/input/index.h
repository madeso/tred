#pragma once

// functions to convert keys and buttons to indexes and back

#include <type_traits>


namespace input
{


template<typename T>
int ToIndex(T k)
{
    static_assert(sizeof(T) <= sizeof(int) && std::is_unsigned_v<T> == false);
    return static_cast<int>(k);
}


template<typename T>
T FromIndex(int index)
{
    static_assert(sizeof(T) <= sizeof(int) && std::is_unsigned_v<T> == false);
    return static_cast<T>(index);
}


}
