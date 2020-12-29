#pragma once

#include <vector>
#include <type_traits>

#include "tred/to_base.h"
#include "tred/types.h"


template<typename T>
constexpr T OnlyF(int size)
{
    return size == 0? 0x0 : (OnlyF<T>(size-1) << 4) | 0xF;
}


template<typename T, typename Id, typename Version, int IdSize=sizeof(Id), int VersionSize=sizeof(Version)>
struct HandleFunctions
{
    using THandle = T;
    using TId = Id;
    using TVersion = Version;
    
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

template<typename E>
using HandleFunctions64 = HandleFunctions<E, u32, u16, 5, 3>;


template<typename T, typename F>
struct HandleVector
{
    using TSelf = HandleVector<T, F>;
    using TData = T;
    using TFunctions = F;
    using THandle = typename TFunctions::THandle;
    using TId = typename TFunctions::TId;
    using TVersion = typename TFunctions::TVersion;

    constexpr static TVersion EMPTY_VERSION = 0;
    constexpr static TVersion FIRST_VERSION = 1;

    struct Pair
    {
        TData data;
        TVersion version;

        Pair()
            : version(EMPTY_VERSION)
        {
        }

        explicit Pair(TVersion v)
            : version(v)
        {
        }
    };
    
    using TVec = std::vector<Pair>;

    TVec vector;

    THandle Add()
    {
        const TVersion version = FIRST_VERSION;
        const auto index = vector.size();
        vector.emplace_back(version);
        assert(vector[index].version == version && "pair constructor failed");
        return TFunctions::Compress(static_cast<TId>(index), version);
    }

    void Clear()
    {
        vector.clear();
    }

    TData& operator[](THandle handle)
    {
        const auto id = static_cast<size_t>(TFunctions::GetId(handle));
        const auto version = TFunctions::GetVersion(handle);
        assert(vector[id].version == version && "invalid handle (use after free)");
        return vector[id].data;
    }

    struct Iterator
    {
        TSelf* vector;
        size_t index;

        Iterator(TSelf* v, size_t i)
            : vector(v)
            , index(i)
        {
        }

        Iterator operator++()
        {
            Iterator r = *this;
            index += 1;
            return r;
        }

        Iterator& operator++(int)
        {
            index += 1;
            return *this;
        }

        TData& operator*()
        {
            return vector->vector[index].data;
        }

        bool operator==(const Iterator& rhs) const
        {
            return vector == rhs.vector && index == rhs.index;
        }

        bool operator!=(const Iterator& rhs) const
        {
            return vector != rhs.vector && index != rhs.index;
        }
    };

    Iterator begin()
    {
        return {this, 0};
    }

    Iterator end()
    {
        return {this, vector.size() + 1};
    }
};
