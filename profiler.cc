#include "profiler.h"

#include <vector>

#include "imgui.h"


ProfileId
GenerateProfilerId()
{
    static ProfileId next_index = static_cast<ProfileId>(0);
    const auto last_index = next_index;
    next_index = static_cast<ProfileId>(next_index + 1);
    return last_index;
}


namespace
{
    struct ProfileRecord
    {
        float total_time = 0.0f;
        std::string_view name = "";
        int hits = 0;
    };

    using ProfileRecords = std::vector<ProfileRecord>;

    ProfileRecords& GetRecords()
    {
        static auto records = ProfileRecords{};
        return records;
    }

    void AddRecord(ProfileId id, float time_taken, std::string_view name)
    {
        auto& rs = GetRecords();
        const auto wanted_size = static_cast<size_t>(id + 1);
        if(rs.size() < wanted_size)
        {
            rs.resize(wanted_size);
        }
        auto& r = rs[static_cast<size_t>(id)];
        r.total_time += time_taken;
        r.name = name;
        r.hits += 1;
    }
}


ProfileScope::ProfileScope(ProfileId i, std::string_view n)
    : id(i)
    , name(n)
    , start(Clock::now())
{
}


ProfileScope::~ProfileScope()
{
    const auto end = Clock::now();
    std::chrono::duration<float> time_taken = end-start;
    AddRecord(id, time_taken.count(), name);
}

void
PrintProfiles()
{
    if(ImGui::Begin("Profiler") == false) { ImGui::End(); return; }

    for(auto& p: GetRecords())
    {
        const float time_per_sample = p.hits > 0 ? p.total_time / static_cast<float>(p.hits) : 0.0f;
        ImGui::Text("%s | %f", p.name.data(), static_cast<double>(time_per_sample));

        p.total_time = 0.0f;
        p.hits = 0;
    }

    ImGui::End();
}
