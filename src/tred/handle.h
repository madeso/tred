#pragma once

#include "tred/to_base.h"

#include <type_traits>


template<typename T>
constexpr T OnlyF(int size)
{
    return size == 0? 0x0 : (OnlyF<T>(size-1) << 4) | 0xF;
}


template<typename T, typename Id, typename Version, int IdSize=sizeof(Id), int VersionSize=sizeof(Version)>
struct HandleFunctions
{
    using Base = typename std::underlying_type<T>::type;

    static_assert
    (
        std::is_unsigned_v<Base> &&
        std::is_unsigned_v<Id> &&
        std::is_unsigned_v<Version>,
        "types needs to be unsigned"
    );
    static_assert(IdSize > 0 && VersionSize > 0, "invalid size");
    static_assert(sizeof(Base) == IdSize + VersionSize, "sizes doesn't match base size");

    static constexpr Base IdMask = OnlyF<Base>(IdSize);
    static constexpr int IdShift = VersionSize*4;
    static constexpr Base VersionMask = OnlyF<Base>(VersionSize);

    static constexpr Id GetId(T t)
    {
        const Base b = to_base(t);
        return static_cast<Id>((b & (IdMask << IdShift)) >> IdShift);
    }

    static constexpr Version GetVersion(T t)
    {
        const Base b = to_base(t);
        return static_cast<Version>(b & VersionMask);
    }

    static constexpr T Compress(Id id, Version version)
    {
        return static_cast<T>(((id & IdMask) << IdShift) | (version & VersionMask));
    }
};

