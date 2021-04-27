#pragma once

#include <chrono>


void
print_profiles();


struct frame_profile_scope
{
    using clock = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock>;
    time_point start;

    frame_profile_scope();
    ~frame_profile_scope();
};


#define PROFILE_FRAME() auto dnu__profiler = FrameProfileScope{}

