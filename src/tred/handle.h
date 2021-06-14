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


template<typename T, typename Id, typename Version, int IdSize=sizeof(Id), int VersionSize=sizeof(Version)>
struct handle_functions
{
    using handle = T;
    using id = Id;
    using version = Version;

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
using handle_functions64 = handle_functions<E, u32, u16, 5, 3>;

template<typename TData, typename TVersion, TVersion TEmptyVersion>
struct handle_vector_pair
{
    TData data;
    TVersion version;
    bool in_use;

    handle_vector_pair()
        : version(TEmptyVersion)
        , in_use(false)
    {
    }

    explicit handle_vector_pair(TVersion v)
        : version(v)
        , in_use(true)
    {
    }
};

template <typename TSelf, typename TData>
struct handle_vector_iterator
{
    TSelf* vector;
    std::size_t index;

    using self = handle_vector_iterator<TSelf, TData>;

    handle_vector_iterator(TSelf* v, std::size_t i)
        : vector(v)
        , index(i)
    {
        advance_until_current_is_used();
    }

    handle_vector_iterator operator++()
    {
        self r = *this;
        index += 1;
        advance_until_current_is_used();
        return r;
    }

    handle_vector_iterator& operator++(int)
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
struct handle_vector_pair_iterator
{
    TSelf* vector;
    std::size_t index;

    using self = handle_vector_pair_iterator<TSelf, THandle, TData, TFunctions, TId>;

    handle_vector_pair_iterator(TSelf* v, std::size_t i)
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
struct handle_vector_pair_iterator_container
{
    TSelf* self;

    explicit handle_vector_pair_iterator_container(TSelf* s) : self(s) {}

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
struct handle_vector
{
    using self = handle_vector<T, F>;
    using value_type = T;
    using functions = F;
    using handle = typename functions::handle;
    using id = typename functions::id;
    using version = typename functions::version;

    constexpr static version EMPTY_VERSION = 0;
    constexpr static version FIRST_VERSION = 1;

    using pair = handle_vector_pair<value_type, version, EMPTY_VERSION>;
    using iterator = handle_vector_iterator<self, value_type>;
    using pair_iterator = handle_vector_pair_iterator<self, handle, value_type, functions, id>;
    using pair_iterator_container = handle_vector_pair_iterator_container<self, pair_iterator>;

    using vector = std::vector<pair>;

    vector data;
    std::vector<handle> free_handles;

    handle create_new_handle()
    {
        if(free_handles.empty() == false)
        {
            const auto handle = *free_handles.rbegin();
            free_handles.pop_back();
            get_pair(handle).in_use = true;
            return handle;
        }
        else
        {
            const version version = FIRST_VERSION;
            const auto index = data.size();
            data.emplace_back(version);
            assert(data[index].version == version && "pair constructor failed");
            assert(data[index].in_use == true && "pair constructor failed");
            return functions::compress(static_cast<id>(index), version);
        }
    }

    void mark_for_reuse(handle handle)
    {
        auto& p = get_pair(handle);
        p.in_use = false;

        const auto id = functions::get_id(handle);

        p.version += 1;
        if(p.version != functions::version_mask)
        {
            // if the version is not max reuse it
            free_handles.emplace_back(functions::compress(id, p.version));
        }
    }

    void clear()
    {
        data.clear();
    }

    pair& get_pair(handle handle)
    {
        const auto id = static_cast<std::size_t>(functions::get_id(handle));
        const auto version = functions::get_version(handle);
        assert(data[id].version == version && "invalid handle (use after free)");
        return data[id];
    }

    value_type& operator[](handle handle)
    {
        assert(get_pair(handle).in_use == true);
        return get_pair(handle).data;
    }

    iterator begin()
    {
        return {this, 0};
    }

    iterator end()
    {
        return {this, data.size()};
    }

    pair_iterator_container as_pairs()
    {
        return pair_iterator_container{this};
    }
};

