#pragma once


#include <algorithm>
#include <vector>


namespace input
{


template<typename T, typename TIsJoystick, typename TCanDetect>
bool mapping_detection(const std::vector<T>& definitions, TIsJoystick&& is_joystick, TCanDetect&& can_detect)
{
    /*
    Both joysticks and non-joysticks are in the same vector
    All joysticks must be detected for the mapping to be detected
    but(!) only one of the non-joysticks is required for the mapping to be detected
    ie activate keyboard _mouse_ for the keyboard+mouse to become active

    J: joystick, D: detected

      J   D      all   any
    ------------------------------------------------
      0   0       1     0
      0   1       1     1

      1   0       0     1
      1   1       1     1
    */


    if(definitions.empty())
    {
        return false;
    }

    const auto joysticks_valid = std::all_of(definitions.begin(), definitions.end(),
        [&](const T& def) -> bool
        {
            return is_joystick(def) == false || can_detect(def);
        }
    );

    const auto non_joysticks_valid = std::any_of(definitions.begin(), definitions.end(),
        [&](const T& def) -> bool
        {
            return !(is_joystick(def) == false && can_detect(def) == false);
        }
    );

    return joysticks_valid && non_joysticks_valid;
}



}
