#ifndef CATCHY_FALSE_STRING_H
#define CATCHY_FALSE_STRING_H

#include <string>
#include <ostream>

namespace catchy
{
    // represents a error (false) or empty string (true)
    struct FalseString
    {
        static
        FalseString
        False(const std::string& text);

        static
        FalseString
        True();

        operator bool() const;
    
        std::string reason;
    };

    std::ostream&
    operator<<(std::ostream& s, const FalseString& f);
}

#endif  // CATCHY_FALSE_STRING_H

