#include "profiler.h"

#include <vector>

#include "imgui.h"

#include "cint.h"


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

    struct ProfileRecords
    {
        std::vector<ProfileRecord> records;
        int selected = -1;
    };

    ProfileRecords& GetRecords()
    {
        static auto records = ProfileRecords{};
        return records;
    }

    void AddRecord(ProfileId id, float time_taken, std::string_view name)
    {
        auto& rs = GetRecords();
        const auto wanted_size = static_cast<size_t>(id + 1);
        if(rs.records.size() < wanted_size)
        {
            rs.records.resize(wanted_size);
        }
        auto& r = rs.records[static_cast<size_t>(id)];
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

    ImGui::Columns(4, "profiling");
    ImGui::Separator();
    ImGui::Text("Function"); ImGui::NextColumn();
    ImGui::Text("Time per sample"); ImGui::NextColumn();
    ImGui::Text("Time"); ImGui::NextColumn();
    ImGui::Text("Hits"); ImGui::NextColumn();
    ImGui::Separator();
    
    auto& rs = GetRecords();
    for(int i=0; i < Csizet_to_int(rs.records.size()); i+=1)
    {
        constexpr float in_ms = 1000.0f;
        auto& p = rs.records[Cint_to_sizet(i)];

        // todo(Gustav): fix sending string_view as .data()
        if (ImGui::Selectable(p.name.data(), rs.selected == i, ImGuiSelectableFlags_SpanAllColumns))
            rs.selected = i;
        ImGui::NextColumn();
        const float time_per_sample = p.hits > 0 ? p.total_time / static_cast<float>(p.hits) : 0.0f;
        ImGui::Text("%.1f", static_cast<double>(time_per_sample * in_ms));ImGui::NextColumn();
        ImGui::Text("%.1f", static_cast<double>(p.total_time * in_ms));ImGui::NextColumn();
        ImGui::Text("%d", p.hits);ImGui::NextColumn();

        p.total_time = 0.0f;
        p.hits = 0;
    }
    ImGui::Columns(1);

    ImGui::End();
}
