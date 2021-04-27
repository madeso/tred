#pragma once

#include <optional>
#include <string>


template<typename T, typename E=std::string>
struct result
{
    std::optional<T> value;
    std::optional<E> e;

    result(T&& tt) : value(std::move(tt)) {}
    result(const E& ee) : e(ee) {}

    operator bool() const { return value.has_value(); }

    E get_error(const E& missing = E{}) const
    {
        if(e) { return *e; }
        else { return missing; }
    }
};

