#include "tred/handle.h"

#include "catch.hpp"

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



