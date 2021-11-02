#include "tred/handle.h"

#include "fmt/format.h"

#include "catch.hpp"
#include "catchy/vectorequals.h"

#include "tred/to_base.h"


TEST_CASE("only f", "[internal]")
{
    CHECK(only_f_hex<u32>(0) == 0x0);
    CHECK(only_f_hex<u32>(1) == 0xF);
    CHECK(only_f_hex<u32>(2) == 0xFF);
    CHECK(only_f_hex<u32>(4) == 0xFFFF);
}


TEST_CASE("handle 8 + 8 = 16", "[handle]")
{
    enum class e64 : u64 {};
    using F = HandleFunctions<e64, u32, u32>;

    SECTION("0")
    {
        auto c = F::compress(0, 0);
        CHECK(to_base(c) == 0);
        CHECK(F::get_id(c) == 0);
        CHECK(F::get_version(c) == 0);
    }

    SECTION("42, 24")
    {
        auto c = F::compress(42, 24);
        CHECK(to_base(c) != 0);
        CHECK(F::get_id(c) == 42);
        CHECK(F::get_version(c) == 24);
    }

    SECTION("0, 42")
    {
        auto c = F::compress(0, 42);
        CHECK(to_base(c) == 42);
        CHECK(F::get_id(c) == 0);
        CHECK(F::get_version(c) == 42);
    }
}


TEST_CASE("handle 5 + 3 = 8", "[handle]")
{
    enum class e32 : u64 {};
    using F = HandleFunctions<e32, u32, u16, 5, 3>;

    SECTION("0")
    {
        auto c = F::compress(0, 0);
        CHECK(to_base(c) == 0);
        CHECK(F::get_id(c) == 0);
        CHECK(F::get_version(c) == 0);
    }

    SECTION("42, 24")
    {
        auto c = F::compress(42, 24);
        CHECK(to_base(c) != 0);
        CHECK(F::get_id(c) == 42);
        CHECK(F::get_version(c) == 24);
    }

    SECTION("0, 42")
    {
        auto c = F::compress(0, 42);
        CHECK(to_base(c) == 42);
        CHECK(F::get_id(c) == 0);
        CHECK(F::get_version(c) == 42);
    }
}



template<typename T, typename V>
std::vector<T> Extract(V& v)
{
    std::vector<T> r;
    for(const auto& t: v)
    {
        r.emplace_back(t);
    }
    return r;
}


template<typename T>
catchy::FalseString VectorEquals
(
    const std::vector<T>& lhs,
    const std::vector<T>& rhs
)
{
    return catchy::VectorEquals(lhs, rhs,
        [](const T& t) { return fmt::format("{}", t);},
        [](const T& l, const T& r) -> catchy::FalseString
        {
            if(l != r)
            {
                return catchy::FalseString::False(fmt::format("{} != {}", l, r));
            }
            else
            {
                return catchy::FalseString::True();
            }
        }
    );
}



TEST_CASE("handle vector", "[handle]")
{
    enum class H : u64 {};
    using F = HandleFunctions64<H>;
    HandleVector<int, F> v;

    CHECK(v.begin().index == 0);
    CHECK(v.end().index == 0);
    CHECK(v.begin() == v.end());
    CHECK(v.begin().index == v.end().index);

    const auto a = v.create_new_handle();
    const auto b = v.create_new_handle();
    const auto c = v.create_new_handle();

    CHECK(v.begin().index == 0);
    CHECK(v.end().index == 3);
    CHECK(v.begin() != v.end());
    CHECK(v.begin().index != v.end().index);

    v[a] = 1;
    v[b] = 2;
    v[c] = 3;

    CHECK(v.data.size() == 3);
    CHECK(v.free_handles.size() == 0);

    REQUIRE(VectorEquals(Extract<int>(v), {1, 2, 3}));

    SECTION("remove start")
    {
        v.mark_for_reuse(a);
        REQUIRE(VectorEquals(Extract<int>(v), {2, 3}));

        const auto d = v.create_new_handle();
        v[d] = 4;
        CHECK(VectorEquals(Extract<int>(v), {4, 2, 3}));
    }

    SECTION("remove middle")
    {
        v.mark_for_reuse(b);
        REQUIRE(VectorEquals(Extract<int>(v), {1, 3}));

        const auto d = v.create_new_handle();
        v[d] = 4;
        CHECK(VectorEquals(Extract<int>(v), {1, 4, 3}));
    }

    SECTION("remove end")
    {
        v.mark_for_reuse(c);
        REQUIRE(VectorEquals(Extract<int>(v), {1, 2}));

        const auto d = v.create_new_handle();
        v[d] = 4;
        CHECK(VectorEquals(Extract<int>(v), {1, 2, 4}));
    }

    SECTION("remove all in order")
    {
        v.mark_for_reuse(a);
        v.mark_for_reuse(b);
        v.mark_for_reuse(c);
        REQUIRE(VectorEquals(Extract<int>(v), {}));
        CHECK(v.free_handles.size() == 3);
        CHECK(v.data.size() == 3);

        const auto d = v.create_new_handle();
        v[d] = 4;

        const auto e = v.create_new_handle();
        v[e] = 5;

        const auto f = v.create_new_handle();
        v[f] = 6;

        CHECK(VectorEquals(Extract<int>(v), {6, 5, 4}));
        CHECK(v.free_handles.size() == 0);
        CHECK(v.data.size() == 3);
    }

    SECTION("remove all in reverse")
    {
        v.mark_for_reuse(c);
        v.mark_for_reuse(b);
        v.mark_for_reuse(a);
        REQUIRE(VectorEquals(Extract<int>(v), {}));
        CHECK(v.free_handles.size() == 3);
        CHECK(v.data.size() == 3);

        const auto d = v.create_new_handle();
        v[d] = 4;

        const auto e = v.create_new_handle();
        v[e] = 5;

        const auto f = v.create_new_handle();
        v[f] = 6;

        CHECK(VectorEquals(Extract<int>(v), {4, 5, 6}));
        CHECK(v.free_handles.size() == 0);
        CHECK(v.data.size() == 3);
    }

    SECTION("pair")
    {
        auto h = std::vector<H>{};
        auto i = std::vector<int>{};

        for(const auto& p: v.as_pairs())
        {
            h.emplace_back(p.first);
            i.emplace_back(p.second);
        }

        REQUIRE(VectorEquals(Extract<int>(v), {1, 2, 3}));
        CHECK(VectorEquals(i, {1, 2, 3}));
        REQUIRE(h.size() == 3);
        CHECK(h[0] == a);
        CHECK(h[1] == b);
        CHECK(h[2] == c);
    }
}

