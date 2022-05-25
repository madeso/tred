#pragma once

#include <cstdint>
#include <string_view>

// implements fnv-1a: https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function

using U64 = std::uint64_t;

constexpr U64
hash64(const std::string_view str, U64 hash = 0xcbf29ce484222325)
{
    return str.empty() ? hash : hash64(str.substr(1), (hash ^ static_cast<U64>(str[0])) * 0x100000001b3);
}
