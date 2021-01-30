#include <cmath>


namespace input
{


float SmoothRange(float s, float half_deadzone)
{
    if(s > 1.0f - half_deadzone) { return 1.0f; }
    else if(s < half_deadzone) { return 0.0f; }
    else { return s; }
}

float SmoothAxis(float s, float half_deadzone)
{
    if(s > 1.0f - half_deadzone) { return 1.0f; }
    else if(s < -1.0f + half_deadzone) { return -1.0f; }
    else if( std::fabs(s) < half_deadzone ) { return 0.0f;}
    else { return s; }
}


}

