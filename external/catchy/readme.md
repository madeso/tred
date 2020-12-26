# catchy
Catchy is a extension for the catch2 library that I have copied from project to project

# Features

## overridable approx for custom types

```cpp
#include "catchy/approx.h"

template <>
bool approximately_equal(const vec3f& lhs, const vec3f& rhs, const ApproxData& data)
{
    return approximately_equal(lhs.x, rhs.x, data)
        && approximately_equal(lhs.y, rhs.y, data)
        && approximately_equal(lhs.z, rhs.z, data);
}
```

## false strings

A boolean that is also a string(reason) when false. Base for building comparrison operators with error information

```cpp
#include "catchy/falsestring.h"

using namespace catchy;

CHECK(FalseString::True());
CHECK_FALSE(FalseString::False("oh noes"));
```

## string equality

Uses false strings. prints out more info where the strig failed such as length difference, first invalid character etc...

```cpp
using namespace catchy;

CHECK(StringEq(foo(), "string"));
CHECK(StringEq(bar(), {"some", "strings"}));
```

## vector equals

Uses false strings, compares arrays

```cpp
using namespace catchy;

FalseString
VectorEquals(const std::vector<unsigned int> lhs, const std::vector<unsigned int>& rhs)
{
    return VectorEquals
    (
        lhs,
        rhs,
        [](unsigned int f) -> std::string { return Str() << f; },
        [](unsigned int a, unsigned int b) -> FalseString
        {
            if(a == b)
            {
                return FalseString::True();
            }
            else
            {
                return FalseString::False
                (
                    Str() << a << " != " << b
                );
            }
        }
    );
}

CHECK(VectorEquals(lhs, rhs));
```
