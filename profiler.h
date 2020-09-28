#ifndef PROFILER_H
#define PROFILER_H

#include <string_view>
#include <chrono>

enum ProfileId : int;

ProfileId
GenerateProfilerId();

void
PrintProfiles();

struct ProfileScope
{
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    ProfileId id;
    std::string_view name;
    TimePoint start;

    ProfileScope(ProfileId i, std::string_view n);
    ~ProfileScope();
};

#define SCOPE_NAME_LINE2(line) scope##line
#define SCOPE_NAME_LINE(line) SCOPE_NAME_LINE2(line)
#define SCOPE_NAME SCOPE_NAME_LINE(__LINE__)
#define PROFILE_SCOPE(name) auto SCOPE_NAME = ProfileScope{[]{static const auto id = ::GenerateProfilerId(); return id;}(), name}
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTIONSIG__)


#endif // PROFILER_H
