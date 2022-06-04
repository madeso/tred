#pragma once


#include <type_traits>
#include "tred/assert.h"


#include "tred/to_base.h"
#include "tred/types.h"

/*

This module implements handles inspired by:
    https://floooh.github.io/2018/06/17/handles-vs-pointers.html
    https://seanmiddleditch.github.io/dangers-of-stdshared-ptr/

The theory is that a system only exposes a non-owning handle (uint)/"weak pointer"
to the outside and functions that operate on such handle.

`HandleFunctions` has functions that splits that typesafe handle into a id
and version. This is useful for all systems that are using handles.

'HandleVector' is a general container for the internal structure. A generic
container that is good enough but can easily be thrown out for a more
optimized thing if needed. 

*/


namespace detail
{
    template<typename T>
    constexpr T only_f_hex(int size)
    {
        return size == 0? 0x0 : (only_f_hex<T>(size-1) << 4) | 0xF;
    }
}


/** Exposes a slot(id) + generation(version) for a handle
 * @param T the handle type, usually soemthing like `enum class Handle : u64 {};` for typesafety
 * @param TId the type of the id
 * @param TVersion the type of the version
 * @param TIdSize how many bytes of T to use when storing the id
 * @param TVersionSize how many bytes of T to use when storing the version
 * 
 * @see HandleFunctions64
 */
template<typename T, typename TId, typename TVersion, int TIdSize=sizeof(TId), int TVersionSize=sizeof(TVersion)>
struct HandleFunctions
{
    using Handle = T;
    using Id = TId;
    using Version = TVersion;

    static constexpr int IdSize = TIdSize;
    static constexpr int VersionSize = TVersionSize;

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

    static constexpr Base id_mask = detail::only_f_hex<Base>(IdSize);
    static constexpr int id_shift = VersionSize*4;
    static constexpr Base version_mask = detail::only_f_hex<Base>(VersionSize);

    static constexpr Id get_id(T t)
    {
        const Base b = to_base(t);
        return static_cast<Id>((b & (id_mask << id_shift)) >> id_shift);
    }

    static constexpr Version get_version(T t)
    {
        const Base b = to_base(t);
        return static_cast<Version>(b & version_mask);
    }

    static constexpr T compress(Id id, Version version)
    {
        return static_cast<T>(((id & id_mask) << id_shift) | (version & version_mask));
    }
};


template<typename E>
using HandleFunctions64 = HandleFunctions<E, u32, u16, 5, 3>;


namespace detail
{
    template<typename TData, typename TVersion, TVersion TEmptyVersion>
    struct HandleVectorPair
    {
        std::optional<TData> data;
        TVersion version;

        HandleVectorPair()
            : data(std::nullopt)
            , version(TEmptyVersion)
        {
        }

        explicit HandleVectorPair(TData&& d, TVersion v)
            : data(std::move(d))
            , version(v)
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
            while (index < vector->data.size() && vector->data[index].data.has_value() == false)
            {
                index += 1;
            }
        }

        TData& operator*()
        {
            return *vector->data[index].data;
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
            while (index < vector->data.size() && vector->data[index].data.has_value() == false)
            {
                index += 1;
            }
        }

        std::pair<THandle, TData&> operator*()
        {
            auto& r = vector->data[index];
            return { TFunctions::compress(static_cast<TId>(index), r.version), *r.data };
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
}

/** Dynamic container. Semi-continous with stable pointers and assertable handles.
 * @param T the type to contain
 * @param F the HandleFunction of the handle
 * @see HandleFunctions
 */
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

    using Pair = detail::HandleVectorPair<ValueType, Version, EMPTY_VERSION>;
    using Iterator = detail::HandleVectorIterator<Self, ValueType>;
    using ConstIterator = detail::HandleVectorIterator<const Self, const ValueType>;
    using PairIterator = detail::HandleVectorPairIterator<Self, Handle, ValueType, Functions, Id>;
    using PairIteratorContainer = detail::HandleVectorPairIteratorContainer<Self, PairIterator>;

    using Vector = std::vector<Pair>;

    Vector data;
    std::vector<Handle> free_handles;

    Handle add(ValueType&& val)
    {
        if(free_handles.empty() == false)
        {
            const auto h = *free_handles.rbegin();
            free_handles.pop_back();
            get_pair(h).data = std::move(val);
            return h;
        }
        else
        {
            const Version v = FIRST_VERSION;
            const auto index = data.size();
            data.emplace_back(std::move(val), v);
            ASSERT(data[index].version == v && "pair constructor failed");
            ASSERT(data[index].data.has_value() == true && "pair constructor failed");
            return Functions::compress(static_cast<Id>(index), v);
        }
    }

    void remove(Handle h)
    {
        auto& p = get_pair(h);
        p.data = {};

        const auto i = Functions::get_id(h);

        // increase the internal version, this should invalidate all existing handles
        p.version += 1;

        if(p.version != Functions::version_mask)
        {
            // if the version is not max reuse it
            free_handles.emplace_back(Functions::compress(i, p.version));
        }
    }

    /** Remove all objects.
     * Please not ehat existing handles are invalidated but this can't be checked.
     * Only call this when all handles are safely removed.
    */
    void clear()
    {
        data.clear();
        free_handles.clear();
    }

    Pair& get_pair(Handle h)
    {
        const auto i = static_cast<std::size_t>(Functions::get_id(h));
        [[maybe_unused]] const auto v = Functions::get_version(h);
        ASSERT(data[i].version == v && "invalid handle: use after free");
        ASSERT(i < data.size() && "invalid handle: use after free (clear)");
        return data[i];
    }

    const Pair& get_pair(Handle h) const
    {
        const auto i = static_cast<std::size_t>(Functions::get_id(h));
        [[maybe_unused]] const auto v = Functions::get_version(h);
        ASSERT(data[i].version == v && "invalid handle: use after free");
        ASSERT(i < data.size() && "invalid handle: use after free (clear)");
        return data[i];
    }

    ValueType& operator[](Handle h)
    {
        ASSERT(get_pair(h).data.has_value() == true);
        return *get_pair(h).data;
    }

    const ValueType& operator[](Handle h) const
    {
        ASSERT(get_pair(h).data.has_value() == true);
        return *get_pair(h).data;
    }

    Iterator begin()
    {
        return {this, 0};
    }

    Iterator end()
    {
        return {this, data.size()};
    }

    ConstIterator begin() const
    {
        return {this, 0};
    }

    ConstIterator end() const
    {
        return {this, data.size()};
    }

    PairIteratorContainer as_pairs()
    {
        return PairIteratorContainer{this};
    }
};


template<typename Type, typename Id>
using HandleVector64 = HandleVector<Type, HandleFunctions64<Id>>;
