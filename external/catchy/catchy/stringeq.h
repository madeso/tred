#ifndef CATCHY_STRINGEQ_H
#define CATCHY_STRINGEQ_H

#include <vector>
#include <string>

#include "catchy/falsestring.h"

namespace catchy
{
    FalseString
    StringEq(const std::vector<std::string> lhs, const std::vector<std::string> rhs);

    FalseString
    StringEq(const std::string& lhs, const std::string& rhs);
}  // namespace euphoria::tests

#endif  // CATCHY_STRINGEQ_H
