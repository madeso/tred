#include "sprites/onebit_font.h"


#include "tred/texture.h"
#include "tred/game.h"

#include "sprites/onebit.h"

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


    void simple_text(sprite_batch* batch, texture* onebit, const glm::vec4 color, float x, float y, const std::string& text_to_draw, const text_animation& anim)
    {
        constexpr auto sprite = rect{font_size, font_size};

        int position_in_string = 0;

        for(char c: text_to_draw)
        {
            if(c == ' ')
            {
                x += font_spacing;
                position_in_string += 1;
            }
            else
            {
                // silently ignore missing characters
                if(auto index = ::onebit::text_string.find(c); index != std::string_view::npos)
                {
                    batch->quad(onebit, anim.transform(position_in_string, sprite.translate(x, y)), ::onebit::text[static_cast<std::size_t>(index)], color);
                    x += font_spacing;
                    position_in_string += 1;
                }
            }
        }
    }

    float get_width_of_string(const std::string& text_to_draw)
    {
        return static_cast<float>(text_to_draw.length()) * font_spacing;
    }

}
