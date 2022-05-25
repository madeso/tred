#pragma once

#include <string>

#include "fmt/format.h"

#include "tred/hash.h"


struct Hashed
{
    constexpr explicit Hashed(U64 h)
        : hash( h )
    {
    }

    U64 hash;
};

struct HashedStringView : Hashed
{
    constexpr HashedStringView(const std::string_view str)
        : Hashed( hash64(str) )
        , text(str)
    {
    }

    std::string_view text;
};

struct HashedString : Hashed
{
    HashedString(const std::string& str);
    HashedString(const HashedStringView& sv);

    std::string text;
};



template<>
struct fmt::formatter<HashedString>: formatter<std::string>
{
    template <typename FormatContext>
    auto format(HashedString s, FormatContext& ctx)
    {
        return formatter<std::string>::format(s.text, ctx);
    }
};

template <>
struct fmt::formatter<HashedStringView>: formatter<std::string_view>
{
  // parse is inherited from formatter<string_view>.
  template <typename FormatContext>
  auto format(HashedStringView s, FormatContext& ctx) {
    return formatter<std::string_view>::format(s.text, ctx);
  }
};


template<> struct std::hash<Hashed>          { std::size_t operator()(const Hashed& h) const noexcept { return h.hash; } };
template<> struct std::hash<HashedString>    { std::size_t operator()(const Hashed& h) const noexcept { return h.hash; } };
template<> struct std::hash<HashedStringView>{ std::size_t operator()(const Hashed& h) const noexcept { return h.hash; } };

constexpr bool operator==(const Hashed& lhs, const Hashed& rhs)
{
    return lhs.hash == rhs.hash;
}

constexpr bool operator<(const Hashed& lhs, const Hashed& rhs)
{
    return lhs.hash < rhs.hash;
}
