#ifndef CATCHY_MAP_EQUALS_H
#define CATCHY_MAP_EQUALS_H

#include <map>
#include <vector>
#include <string>
#include <sstream>

#include "fmt/format.h"

#include "catchy/falsestring.h"


namespace catchy
{


template<typename K, typename V, typename C>
FalseString
MapEq(const std::map<K, V>& lhs, const std::map<K, V>& rhs, const C& compare)
{
    std::vector<std::string> issues;

    for(const auto& lhsp: lhs)
    {
        auto found = rhs.find(lhsp.first);
        if(found == rhs.end())
        {
            issues.emplace_back(fmt::format("{} missing in rhs", lhsp.first));
        }
        else
        {
            const catchy::FalseString result = compare(lhsp.second, found->second);
            if(result == false)
            {
                issues.emplace_back(fmt::format("{} was different: {}", lhsp.first, result.reason));
            }
        }
    }

    for(const auto& rhsp: rhs)
    {
        auto found = lhs.find(rhsp.first);
        if(found == lhs.end())
        {
            issues.emplace_back(fmt::format("{} missing in lhs", rhsp.first));
        }
    }

    if(issues.empty()) { return FalseString::True();}

    std::ostringstream ss;
    bool first = true;
    for(const auto& s: issues)
    {
        if(first) first = false;
        else ss << "\n";
        ss << s;
    }
    
    return catchy::FalseString::False(ss.str());
};


}


#endif
