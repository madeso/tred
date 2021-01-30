#pragma once


namespace input
{


// smoothes around 0 and 1
float SmoothRange(float s, float half_deadzone = 0.01f);

// smoothes around -1 0 and 1
float SmoothAxis(float s, float half_deadzone = 0.01f);


}

