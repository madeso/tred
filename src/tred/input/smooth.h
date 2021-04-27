#pragma once


namespace input
{

// todo(Gustav): move deadzone to bind

// smoothes around 0 and 1
float smooth_range(float s, float half_deadzone = 0.1f);

// smoothes around -1 0 and 1
float smooth_axis(float s, float half_deadzone = 0.1f);


}

