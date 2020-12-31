#pragma once


namespace input
{


// unidirectional absolute axes (trigger, (analog) buttons)
// bidirectional absolute axes (joy sticks)
// relative axes (mouse axes, wheels, trackballs)

enum class Range
{
    Infinite,
    WithinZeroToOne, // oneway
    WithinNegativeOneToPositiveOne // twoway
};


}
