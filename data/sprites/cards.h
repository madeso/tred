#pragma once

#include "tred/rect.h"

#include <array>

namespace cards
{
    // probably want to move this to a game logic?
    enum class type
    {
        ace, two, three, four, five, six, seven, eight, nine, ten, jack, queen, king
    };

    constexpr rect c(int x, int y) { return rect::from_xywh(static_cast<float>(11 + 64*(x-1)), static_cast<float>(2 + 64*(y-1)), 42, 60); }

    // todo(Gustav): find out a better way to define the card arrays
    constexpr std::array<rect, 13> hearts = {c(1, 1), c(2, 1), c(3, 1), c(4, 1), c(5, 1), c(6, 1), c(7, 1), c(8, 1), c(9, 1), c(10, 1), c(11, 1), c(12, 1), c(13, 1)};

    constexpr rect blank = c(14, 1);
    constexpr rect back = c(14, 2);
    constexpr rect joker1 = c(14, 3);
    constexpr rect joker2 = c(14, 4);

}
