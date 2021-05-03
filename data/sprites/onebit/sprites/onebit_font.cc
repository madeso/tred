#include "sprites/onebit_font.h"

#include <cassert>

#include "tred/texture.h"
#include "tred/game.h"

#include "sprites/onebit.h"

#include "fmt/format.h"

namespace onebit
{
    rect no_text_animation::transform(int, const rect& r) const { return r; }

    siny_animation::siny_animation(float c, float s, float o)
        : change(c)
        , scale(s)
        , offset(o)
    {
    }

    rect siny_animation::transform(int index, const rect& r) const
    {
        const auto a = static_cast<float>(index) * scale + offset;
        constexpr float one_turn = 6.28318530718f; // 2*pi, thanks google
        return r.translate(0.0f, std::sin(a * one_turn) * change);
    }


    void font::simple_text(sprite_batch* batch, texture* onebit, const glm::vec4 color, float x, float y, const std::string& text_to_draw, const text_animation& anim) const
    {
        const auto font_spacing = get_spacing();
        const auto sprite = rect{size, size};

        int position_in_string = 0;
        const auto start_x = x;

        for(char c: text_to_draw)
        {
            if(c == ' ')
            {
                x += font_spacing;
                position_in_string += 1;
            }
            else if(c == '\n')
            {
                x = start_x;
                y += font_spacing;
            }
            else
            {
                auto index = ::onebit::text_string.find(c);
                if(index != std::string_view::npos)
                {
                    batch->quad(onebit, anim.transform(position_in_string, sprite.translate(x, y)), ::onebit::text[static_cast<std::size_t>(index)], color);
                    x += font_spacing;
                    position_in_string += 1;
                }
                else
                {
                    fmt::print("Invalid character {}\n", c);
                    assert(false && "invalid character");
                }
            }
        }
    }

    float font::get_width_of_string(const std::string& text_to_draw) const
    {
        return static_cast<float>(text_to_draw.length()) * get_spacing();
    }

}
