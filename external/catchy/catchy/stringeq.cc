#include "catchy/stringeq.h"

#include <sstream>
#include <algorithm>
#include <string_view>

#include "catchy/vectortostring.h"
#include "catchy/falsestring.h"

#include "catch.hpp"

namespace
{
    std::string EscapeString(const std::string& str)
    {
        return ::Catch::Detail::stringify(str);
    }

    std::string CharToString(char c)
    {
        constexpr std::string_view smart_characters
            = "abcdefghijklmnopqrstuwxyz"
              "ABCDEFGHIJKLMNOPQRSTUWXYZ"
              " "
              "~!@#$%^&*()_+"
              "`123456790-="
              ",.<>/?"
              "{}[]:;\"'\\|"
              "\n\r\t"
            ;
        std::ostringstream ss;
        switch(c)
        {
        case 0:
            ss << "<null>";
            return ss.str();
        case '\n':
            ss << "<\\n>";
            break;
        case '\r':
            ss << "<\\r>";
            break;
        case '\t':
            ss << "<tab>";
            break;
        // source: http://www.asciitable.com/
        case  1: ss << "<start of heading>"; break;
        case  2: ss << "<start of text>"; break;
        case  3: ss << "<end of text>"; break;
        case  4: ss << "<end of transmission>"; break;
        case  5: ss << "<enquiry>"; break;
        case  6: ss << "<acknowledge>"; break;
        case  7: ss << "<bell>"; break;
        case  8: ss << "<backspace>"; break;
        // case  9: ss << "<horizontal tab>"; break;
        // case 10: ss << "<newline>"; break;
        case 11: ss << "<vertical tab>"; break;
        case 12: ss << "<new page>"; break;
        // case 13: ss << "<carriage return>"; break;
        case 14: ss << "<shift out>"; break;
        case 15: ss << "<shift in>"; break;
        case 16: ss << "<data link esqape>"; break;
        case 17: ss << "<device control 1>"; break;
        case 18: ss << "<device control 2>"; break;
        case 19: ss << "<device control 3>"; break;
        case 20: ss << "<device control 4>"; break;
        case 21: ss << "<negative acknowledge>"; break;
        case 22: ss << "<synchronous idle>"; break;
        case 23: ss << "<end of trans. block>"; break;
        case 24: ss << "<cancel>"; break;
        case 25: ss << "<end of medium>"; break;
        case 26: ss << "<substitute>"; break;
        case 27: ss << "<escape>"; break;
        case 28: ss << "<file separator>"; break;
        case 29: ss << "<group separator>"; break;
        case 30: ss << "<record separator>"; break;
        case 31: ss << "<unit separator>"; break;
        case 127: ss << "<DEL>"; break;
        
        case ' ':
            ss << "<space>";
            break;
        default:
            ss << c;
            break;
        }

        if(smart_characters.find(c) == std::string_view::npos)
        {
            ss << "(0x" << std::hex << static_cast<int>(c) << ")";
        }
        return ss.str();
    }

    std::string::size_type
    FindFirstIndexOfMismatch(const std::string& lhs, const std::string& rhs)
    {
        const auto end = std::min(lhs.size(), rhs.size());

        std::string::size_type index = 0;
        for(; index < end; index+=1)
        {
            if(lhs[index]!=rhs[index])
            {
                return index;
            }
        }

        if(index >= lhs.size() && index >= rhs.size())
        {
            return std::string::npos;
        }
        else
        {
            return end;
        }
    }
}

namespace catchy
{
    FalseString
    StringEq(const std::string& lhs, const std::string& rhs)
    {
        const auto s = FindFirstIndexOfMismatch(lhs, rhs);
        assert((s==std::string::npos && lhs == rhs) || (s != std::string::npos && lhs != rhs));
        if(s != std::string::npos )
        {
            std::ostringstream ss;
                
            ss << "lhs: " << EscapeString(lhs) << " and "
               << "rhs: " << EscapeString(rhs) ;
            ss << ", lengths are "
                << lhs.size()
                << " vs "
                << rhs.size();
            ss << ", first diff at " << s << " with "
                << CharToString(lhs[s])
                << "/"
                << CharToString(rhs[s]);
            // ss << ", edit-distance is " << EditDistance(lhs, rhs);

            return FalseString::False(ss.str());
        }

        return FalseString::True();
    }


    FalseString
    StringEq(const std::vector<std::string> lhs, const std::vector<std::string> rhs)
    {
        auto size_equal = FalseString::True();
        if(lhs.size() != rhs.size())
        {
            std::ostringstream ss;
            ss << "Size mismatch: "
               << lhs.size()
               << " vs "
               << rhs.size();
            ss << VectorToString(lhs, EscapeString) << " " << VectorToString(rhs, EscapeString);
            size_equal = FalseString::False(ss.str());
        }

        const auto size = std::min(lhs.size(), rhs.size());
        for(size_t i =0; i < size; i+=1)
        {
            const FalseString equals = StringEq(lhs[i], rhs[i]);
            if(!equals)
            {
                std::ostringstream ss;

                if(!size_equal)
                {
                    ss << size_equal.reason << ", and first invalid";
                }
                else
                {
                    ss << VectorToString(lhs, EscapeString) << "vs" << VectorToString(rhs, EscapeString);
                    ss << "First invalid";
                }


                ss << " value at index ";
                ss << i << ", " << equals.reason;
                return FalseString::False(ss.str());
            }
        }

        if(!size_equal)
        {
            return size_equal;
        }

        return FalseString::True();
    }
}  // namespace euphoria::tests
