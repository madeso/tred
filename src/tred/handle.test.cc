#include "tred/handle.h"

#include "fmt/format.h"

#include "catch.hpp"
#include "catchy/vectorequals.h"

#include "tred/to_base.h"


TEST_CASE("only f", "[internal]")
{
    CHECK(OnlyF<u32>(0) == 0x0);
    CHECK(OnlyF<u32>(1) == 0xF);
    CHECK(OnlyF<u32>(2) == 0xFF);
    CHECK(OnlyF<u32>(4) == 0xFFFF);
}


TEST_CASE("handle 8 + 8 = 16", "[handle]")
{
    enum class e64 : u64 {};
    using F = HandleFunctions<e64, u32, u32>;

    SECTION("0")
    {
        auto c = F::Compress(0, 0);
        CHECK(to_base(c) == 0);
        CHECK(F::GetId(c) == 0);
        CHECK(F::GetVersion(c) == 0);
    }

    SECTION("42, 24")
    {
        auto c = F::Compress(42, 24);
        CHECK(to_base(c) != 0);
        CHECK(F::GetId(c) == 42);
        CHECK(F::GetVersion(c) == 24);
    }

    SECTION("0, 42")
    {
        auto c = F::Compress(0, 42);
        CHECK(to_base(c) == 42);
        CHECK(F::GetId(c) == 0);
        CHECK(F::GetVersion(c) == 42);
    }
}


TEST_CASE("handle 5 + 3 = 8", "[handle]")
{
    enum class e32 : u64 {};
    using F = HandleFunctions<e32, u32, u16, 5, 3>;

    SECTION("0")
    {
        auto c = F::Compress(0, 0);
        CHECK(to_base(c) == 0);
        CHECK(F::GetId(c) == 0);
        CHECK(F::GetVersion(c) == 0);
    }

    SECTION("42, 24")
    {
        auto c = F::Compress(42, 24);
        CHECK(to_base(c) != 0);
        CHECK(F::GetId(c) == 42);
        CHECK(F::GetVersion(c) == 24);
    }

    SECTION("0, 42")
    {
        auto c = F::Compress(0, 42);
        CHECK(to_base(c) == 42);
        CHECK(F::GetId(c) == 0);
        CHECK(F::GetVersion(c) == 42);
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

    const auto a = v.Add();
    const auto b = v.Add();
    const auto c = v.Add();

    CHECK(v.begin().index == 0);
    CHECK(v.end().index == 3);
    CHECK(v.begin() != v.end());
    CHECK(v.begin().index != v.end().index);

    v[a] = 1;
    v[b] = 2;
    v[c] = 3;

    CHECK(v.vector.size() == 3);
    CHECK(v.free_handles.size() == 0);

    REQUIRE(VectorEquals(Extract<int>(v), {1, 2, 3}));

    SECTION("remove start")
    {
        v.Remove(a);
        REQUIRE(VectorEquals(Extract<int>(v), {2, 3}));

        const auto d = v.Add();
        v[d] = 4;
        CHECK(VectorEquals(Extract<int>(v), {4, 2, 3}));
    }

    SECTION("remove middle")
    {
        v.Remove(b);
        REQUIRE(VectorEquals(Extract<int>(v), {1, 3}));

        const auto d = v.Add();
        v[d] = 4;
        CHECK(VectorEquals(Extract<int>(v), {1, 4, 3}));
    }

    SECTION("remove end")
    {
        v.Remove(c);
        REQUIRE(VectorEquals(Extract<int>(v), {1, 2}));

        const auto d = v.Add();
        v[d] = 4;
        CHECK(VectorEquals(Extract<int>(v), {1, 2, 4}));
    }

    SECTION("remove all in order")
    {
        v.Remove(a);
        v.Remove(b);
        v.Remove(c);
        REQUIRE(VectorEquals(Extract<int>(v), {}));
        CHECK(v.free_handles.size() == 3);
        CHECK(v.vector.size() == 3);

        const auto d = v.Add();
        const auto e = v.Add();
        const auto f = v.Add();
        v[d] = 4;
        v[e] = 5;
        v[f] = 6;
        CHECK(VectorEquals(Extract<int>(v), {6, 5, 4}));
        CHECK(v.free_handles.size() == 0);
        CHECK(v.vector.size() == 3);
    }

    SECTION("remove all in reverse")
    {
        v.Remove(c);
        v.Remove(b);
        v.Remove(a);
        REQUIRE(VectorEquals(Extract<int>(v), {}));
        CHECK(v.free_handles.size() == 3);
        CHECK(v.vector.size() == 3);

        const auto d = v.Add();
        const auto e = v.Add();
        const auto f = v.Add();
        v[d] = 4;
        v[e] = 5;
        v[f] = 6;
        CHECK(VectorEquals(Extract<int>(v), {4, 5, 6}));
        CHECK(v.free_handles.size() == 0);
        CHECK(v.vector.size() == 3);
    }
}

