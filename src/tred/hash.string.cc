#include "tred/hash.string.h"


HashedString::HashedString(const std::string& str)
    : Hashed(hash64(str))
    , text(str)
{
}

HashedString::HashedString(const HashedStringView& sv)
    : Hashed(sv.hash)
    , text(sv.text)
{
}
