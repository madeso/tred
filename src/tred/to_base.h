#pragma once

// https://gist.github.com/dirocco/0e23867d0ea9ab2a23dd6dcaba065f53

#include <type_traits>


template<typename E>
constexpr typename std::underlying_type<E>::type to_base(E e) noexcept
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

