#include <array>

template<typename T, T Count>
constexpr std::array<T, static_cast<std::size_t>(Count)> values_in_enum()
{
    std::array<T, static_cast<std::size_t>(Count)> r{{}};
    for(std::size_t i=0; i<static_cast<std::size_t>(Count); i+=1)
    {
        r[i] = static_cast<T>(i);
    }
    return r;
}
