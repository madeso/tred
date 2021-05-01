#pragma once

#include "tred/rect.h"
#include "tred/image.h"

#include <array>

namespace cards
{
    loaded_image load_texture();

    // probably want to move this to a game logic?
    enum class type
    {
        ace, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king
    };

    constexpr recti c(int x, int y) { return recti::from_xywh(11 + 64*(x-1), 2 + 64*(y-1), 42, 60); }

    // todo(Gustav): find out a better way to define the card arrays
    constexpr std::array<recti, 13> hearts = {c(1, 1), c(2, 1), c(3, 1), c(4, 1), c(5, 1), c(6, 1), c(7, 1), c(8, 1), c(9, 1), c(10, 1), c(11, 1), c(12, 1), c(13, 1)};

    constexpr recti blank = c(14, 1);
    constexpr recti back = c(14, 2);
    constexpr recti joker1 = c(14, 3);
    constexpr recti joker2 = c(14, 4);

}
