#include "catchy/falsestring.h"

#include <cassert>

namespace catchy
{
    FalseString FalseString::False(const std::string& reason)
    {
        assert(!reason.empty());
        return {reason};
    }


    FalseString
    FalseString::True()
    {
        return {""};
    }


    FalseString::operator bool() const
    {
        return reason.empty();
    }


    std::ostream&
    operator<<(std::ostream& s, const FalseString& f)
    {
        if(f)
        {
            s << "<true>";
        }
        else
        {
            s << f.reason;
        }
        return s;
    }
}
