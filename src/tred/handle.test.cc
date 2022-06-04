#include "tred/handle.h"



#include "catch.hpp"
#include "catchy/vectorequals.h"

#include "tred/to_base.h"


TEST_CASE("only f", "[internal]")
{
    CHECK(detail::only_f_hex<u32>(0) == 0x0);
    CHECK(detail::only_f_hex<u32>(1) == 0xF);
    CHECK(detail::only_f_hex<u32>(2) == 0xFF);
    CHECK(detail::only_f_hex<u32>(4) == 0xFFFF);
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


TEST_CASE("handle vector (white+blackbox)", "[handle]")
{
    enum class H : u64 {};
    using F = HandleFunctions64<H>;
    HandleVector<int, F> v;

    // validate internal structure/whitebox testing
    CHECK(v.begin().index == 0);
    CHECK(v.end().index == 0);
    CHECK(v.begin() == v.end());
    CHECK(v.begin().index == v.end().index);

    const auto a = v.add(40);
    const auto b = v.add(41);
    const auto c = v.add(42);
    REQUIRE(catchy::VectorEquals(Extract<int>(v), {40, 41, 42}));

    CHECK(v.begin().index == 0);
    CHECK(v.end().index == 3);
    CHECK(v.begin() != v.end());
    CHECK(v.begin().index != v.end().index);

    v[a] = 1;
    v[b] = 2;
    v[c] = 3;

    CHECK(v.data.size() == 3);
    CHECK(v.free_handles.size() == 0);

    REQUIRE(catchy::VectorEquals(Extract<int>(v), {1, 2, 3}));

    SECTION("remove start")
    {
        v.remove(a);
        REQUIRE(catchy::VectorEquals(Extract<int>(v), {2, 3}));

        CHECK(v.data.size() == 3);
        REQUIRE(v.free_handles.size() == 1);

        const auto d = v.add(5);
        REQUIRE(v.free_handles.size() == 0);
        CHECK(catchy::VectorEquals(Extract<int>(v), {5, 2, 3}));

        v[d] = 4;
        CHECK(catchy::VectorEquals(Extract<int>(v), {4, 2, 3}));
    }

    SECTION("remove middle")
    {
        v.remove(b);
        REQUIRE(catchy::VectorEquals(Extract<int>(v), {1, 3}));

        CHECK(v.data.size() == 3);
        REQUIRE(v.free_handles.size() == 1);

        const auto d = v.add(5);
        REQUIRE(v.free_handles.size() == 0);
        CHECK(catchy::VectorEquals(Extract<int>(v), {1, 5, 3}));

        v[d] = 4;
        CHECK(catchy::VectorEquals(Extract<int>(v), {1, 4, 3}));
    }

    SECTION("remove end")
    {
        v.remove(c);
        REQUIRE(catchy::VectorEquals(Extract<int>(v), {1, 2}));

        CHECK(v.data.size() == 3);
        REQUIRE(v.free_handles.size() == 1);

        const auto d = v.add(6);
        REQUIRE(v.free_handles.size() == 0);
        CHECK(catchy::VectorEquals(Extract<int>(v), {1, 2, 6}));

        v[d] = 4;
        CHECK(catchy::VectorEquals(Extract<int>(v), {1, 2, 4}));
    }

    SECTION("remove all in order")
    {
        v.remove(a);
        v.remove(b);
        v.remove(c);
        REQUIRE(catchy::VectorEquals(Extract<int>(v), {}));
        CHECK(v.free_handles.size() == 3);
        CHECK(v.data.size() == 3);

        const auto d = v.add(7);
        const auto e = v.add(8);
        const auto f = v.add(9);

        CHECK(catchy::VectorEquals(Extract<int>(v), {9, 8, 7}));

        v[d] = 4;
        v[e] = 5;
        v[f] = 6;

        CHECK(catchy::VectorEquals(Extract<int>(v), {6, 5, 4}));
        CHECK(v.free_handles.size() == 0);
        CHECK(v.data.size() == 3);
    }

    SECTION("remove all in reverse")
    {
        v.remove(c);
        v.remove(b);
        v.remove(a);
        REQUIRE(catchy::VectorEquals(Extract<int>(v), {}));
        CHECK(v.free_handles.size() == 3);
        CHECK(v.data.size() == 3);

        const auto d = v.add(7);
        const auto e = v.add(8);
        const auto f = v.add(9);

        CHECK(catchy::VectorEquals(Extract<int>(v), {7, 8, 9}));

        v[d] = 4;
        v[e] = 5;
        v[f] = 6;

        CHECK(catchy::VectorEquals(Extract<int>(v), {4, 5, 6}));
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

        CHECK(catchy::VectorEquals(Extract<int>(v), {1, 2, 3}));
        CHECK(catchy::VectorEquals(i, {1, 2, 3}));
        CHECK(catchy::VectorEquals(h, {a, b, c}));
    }
}

