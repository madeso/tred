#pragma once

#include <vector>
#include <type_traits>
#include <cassert>

#include "tred/to_base.h"
#include "tred/types.h"


template<typename T>
constexpr T only_f_hex(int size)
{
    return size == 0? 0x0 : (only_f_hex<T>(size-1) << 4) | 0xF;
}


template<typename T, typename TId, typename TVersion, int TIdSize=sizeof(TId), int TVersionSize=sizeof(TVersion)>
struct HandleFunctions
{
    using Handle = T;
    using Id = TId;
    using Version = TVersion;

    static constexpr int IdSize = TIdSize;
    static constexpr int VersionSize = TVersionSize;

    using base = typename std::underlying_type<T>::type;

    static_assert
    (
        std::is_unsigned_v<base> &&
        std::is_unsigned_v<Id> &&
        std::is_unsigned_v<Version>,
        "types needs to be unsigned"
    );
    static_assert(IdSize > 0 && VersionSize > 0, "invalid size");
    static_assert(sizeof(base) == IdSize + VersionSize, "sizes doesn't match base size");

    static constexpr base id_mask = only_f_hex<base>(IdSize);
    static constexpr int id_shift = VersionSize*4;
    static constexpr base version_mask = only_f_hex<base>(VersionSize);

    static constexpr Id get_id(T t)
    {
        const base b = to_base(t);
        return static_cast<Id>((b & (id_mask << id_shift)) >> id_shift);
    }

    static constexpr Version get_version(T t)
    {
        const base b = to_base(t);
        return static_cast<Version>(b & version_mask);
    }

    static constexpr T compress(Id id, Version version)
    {
        return static_cast<T>(((id & id_mask) << id_shift) | (version & version_mask));
    }
};

template<typename E>
using HandleFunctions64 = HandleFunctions<E, u32, u16, 5, 3>;

template<typename TData, typename TVersion, TVersion TEmptyVersion>
struct HandleVectorPair
{
    TData data;
    TVersion version;
    bool in_use;

    HandleVectorPair()
        : version(TEmptyVersion)
        , in_use(false)
    {
    }

    explicit HandleVectorPair(TVersion v)
        : version(v)
        , in_use(true)
    {
    }
};

template <typename TSelf, typename TData>
struct HandleVectorIterator
{
    TSelf* vector;
    std::size_t index;

    using self = HandleVectorIterator<TSelf, TData>;

    HandleVectorIterator(TSelf* v, std::size_t i)
        : vector(v)
        , index(i)
    {
        advance_until_current_is_used();
    }

    HandleVectorIterator operator++()
    {
        self r = *this;
        index += 1;
        advance_until_current_is_used();
        return r;
    }

    HandleVectorIterator& operator++(int)
    {
        index += 1;
        advance_until_current_is_used();
        return *this;
    }

    void advance_until_current_is_used()
    {
        while (index < vector->data.size() && vector->data[index].in_use == false)
        {
            index += 1;
        }
    }

    TData& operator*()
    {
        return vector->data[index].data;
    }

    bool operator==(const self& rhs) const
    {
        return vector == rhs.vector && index == rhs.index;
    }

    bool operator!=(const self& rhs) const
    {
        return vector != rhs.vector || index != rhs.index;
    }
};

template<typename TSelf, typename THandle, typename TData, typename TFunctions, typename TId>
struct HandleVectorPairIterator
{
    TSelf* vector;
    std::size_t index;

    using self = HandleVectorPairIterator<TSelf, THandle, TData, TFunctions, TId>;

    HandleVectorPairIterator(TSelf* v, std::size_t i)
        : vector(v)
        , index(i)
    {
        advance_until_current_is_used();
    }

    self operator++()
    {
        self r = *this;
        index += 1;
        advance_until_current_is_used();
        return r;
    }

    self& operator++(int)
    {
        index += 1;
        advance_until_current_is_used();
        return *this;
    }

    void advance_until_current_is_used()
    {
        while (index < vector->data.size() && vector->data[index].in_use == false)
        {
            index += 1;
        }
    }

    std::pair<THandle, TData&> operator*()
    {
        auto& r = vector->data[index];
        return { TFunctions::compress(static_cast<TId>(index), r.version), r.data };
    }

    bool operator==(const self& rhs) const
    {
        return vector == rhs.vector && index == rhs.index;
    }

    bool operator!=(const self& rhs) const
    {
        return vector != rhs.vector || index != rhs.index;
    }
};

template<typename TSelf, typename TPairIterator>
struct HandleVectorPairIteratorContainer
{
    TSelf* self;

    explicit HandleVectorPairIteratorContainer(TSelf* s) : self(s) {}

    TPairIterator begin()
    {
        return { self, 0 };
    }

    TPairIterator end()
    {
        return { self, self->data.size() };
    }
};

template<typename T, typename F>
struct HandleVector
{
    using Self = HandleVector<T, F>;
    using ValueType = T;
    using Functions = F;
    using Handle = typename Functions::Handle;
    using Id = typename Functions::Id;
    using Version = typename Functions::Version;

    constexpr static Version EMPTY_VERSION = 0;
    constexpr static Version FIRST_VERSION = 1;

    using Pair = HandleVectorPair<ValueType, Version, EMPTY_VERSION>;
    using Iterator = HandleVectorIterator<Self, ValueType>;
    using PairIterator = HandleVectorPairIterator<Self, Handle, ValueType, Functions, Id>;
    using PairIteratorContainer = HandleVectorPairIteratorContainer<Self, PairIterator>;

    using Vector = std::vector<Pair>;

    Vector data;
    std::vector<Handle> free_handles;

    Handle create_new_handle()
    {
        if(free_handles.empty() == false)
        {
            const auto h = *free_handles.rbegin();
            free_handles.pop_back();
            get_pair(h).in_use = true;
            return h;
        }
        else
        {
            const Version v = FIRST_VERSION;
            const auto index = data.size();
            data.emplace_back(v);
            assert(data[index].version == v && "pair constructor failed");
            assert(data[index].in_use == true && "pair constructor failed");
            return Functions::compress(static_cast<Id>(index), v);
        }
    }

    void mark_for_reuse(Handle h)
    {
        auto& p = get_pair(h);
        p.in_use = false;

        const auto i = Functions::get_id(h);

        p.version += 1;
        if(p.version != Functions::version_mask)
        {
            // if the version is not max reuse it
            free_handles.emplace_back(Functions::compress(i, p.version));
        }
    }

    void clear()
    {
        data.clear();
    }

    Pair& get_pair(Handle h)
    {
        const auto i = static_cast<std::size_t>(Functions::get_id(h));
        const auto v = Functions::get_version(h);
        assert(data[i].version == v && "invalid handle (use after free)");
        return data[i];
    }

    ValueType& operator[](Handle h)
    {
        assert(get_pair(h).in_use == true);
        return get_pair(h).data;
    }

    Iterator begin()
    {
        return {this, 0};
    }

    Iterator end()
    {
        return {this, data.size()};
    }

    PairIteratorContainer as_pairs()
    {
        return PairIteratorContainer{this};
    }
};

