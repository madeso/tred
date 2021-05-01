#pragma once

#include <string>

#include "tred/rect.h"
#include "glm/glm.hpp"

struct texture;
struct sprite_batch;

namespace onebit
{
    // constexpr auto font_size = 12.0f;
    // constexpr auto font_spacing = font_size * 0.7f;

    struct text_animation
    {
        virtual ~text_animation() = default;
        virtual rect transform(int at, const rect& r) const = 0;
    };

    struct no_text_animation : public text_animation
    {
        rect transform(int, const rect& r) const override;
    };

    struct siny_animation : public text_animation
    {
        float change;
        float scale;
        float offset;

        siny_animation(float c, float s, float o);

        rect transform(int index, const rect& r) const override;
    };

    struct font
    {
        float size;

        constexpr float get_spacing() const
        {
            return size * 0.7f;
        }

        void simple_text(sprite_batch* batch, texture* onebit, const glm::vec4 color, float x, float y, const std::string& text, const text_animation& anim) const;

        float get_width_of_string(const std::string& text) const;
    };
}
