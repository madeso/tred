#pragma once

#include <optional>
#include <string>


template<typename T, typename E=std::string>
struct Result
{
    std::optional<T> t;
    std::optional<E> e;

    Result(T&& tt) : t(std::move(tt)) {}
    Result(const E& ee) : e(ee) {}

    operator bool() const { return t.has_value(); }

    T& value()
    {
        return *t;
    }

    const E& error() const
    {
        return *e;
    }
};

