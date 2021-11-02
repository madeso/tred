#pragma once


template<typename F, typename S>
struct SortablePair
{
    using self = SortablePair<F, S>;

    SortablePair(F f, S s)
        : first(f)
        , second(s)
    {
    }

    bool operator<(const self& rhs) const
    {
        if (first == rhs.first)
        {
            return second < rhs.second;
        }
        else
        {
            return first < rhs.first;
        }
    }

    F first;
    S second;
};
