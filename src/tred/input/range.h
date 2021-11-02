#pragma once


namespace input
{


// unidirectional absolute axes (trigger, (analog) buttons)
// bidirectional absolute axes (joy sticks)
// relative axes (mouse axes, wheels, trackballs)

enum class Range
{
    infinite,
    within_zero_to_one, // oneway
    within_negative_one_to_positive_one // twoway
};


}
