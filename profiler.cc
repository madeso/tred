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
        std::string_view name = "";

        float total_time = 0.0f;
        int hits = 0;

        std::vector<float> history_of_total_times;
        std::vector<int> history_of_hits;

        void Backup()
        {
            constexpr std::size_t MAX_SIZE = 100;

            if(history_of_hits.size() >= MAX_SIZE) { history_of_hits.erase(history_of_hits.begin()); }
            if(history_of_total_times.size() >= MAX_SIZE) { history_of_total_times.erase(history_of_total_times.begin()); }
            history_of_hits.push_back(hits);
            history_of_total_times.push_back(total_time);
        }

        void Clear()
        {
            total_time = 0.0f;
            hits = 0;
        }
    };

    struct ProfileRecords
    {
        std::vector<ProfileRecord> records;

        int selected = -1;
        int selected_history = -1;
        bool running = true;
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

    auto& rs = GetRecords();
    constexpr float in_ms = 1000.0f;

    ImGui::Checkbox("Running", &rs.running);

    ImGui::Columns(4, "profiling");
    ImGui::Separator();
    ImGui::Text("Function"); ImGui::NextColumn();
    ImGui::Text("Time per sample"); ImGui::NextColumn();
    ImGui::Text("Time"); ImGui::NextColumn();
    ImGui::Text("Hits"); ImGui::NextColumn();
    ImGui::Separator();

    const auto show_current = !
    (
        rs.running==false
        &&
        (
            rs.records.empty() == false &&
            rs.selected_history >= 0 &&
            rs.selected_history < Csizet_to_int(rs.records[0].history_of_hits.size())
        )
    );
    for(int i=0; i < Csizet_to_int(rs.records.size()); i+=1)
    {
        auto& record = rs.records[Cint_to_sizet(i)];
        
        if(rs.running)
        {
            record.Backup();
        }

        auto hits = show_current ? record.hits : record.history_of_hits[Cint_to_sizet(rs.selected_history)];
        auto total_time = show_current ? record.total_time : record.history_of_total_times[Cint_to_sizet(rs.selected_history)];

        // todo(Gustav): fix sending string_view as .data()
        if (ImGui::Selectable(record.name.data(), rs.selected == i, ImGuiSelectableFlags_SpanAllColumns))
            rs.selected = i;
        ImGui::NextColumn();
        const float time_per_sample = hits > 0 ? total_time / static_cast<float>(hits) : 0.0f;
        ImGui::Text("%.1f", static_cast<double>(time_per_sample * in_ms));ImGui::NextColumn();
        ImGui::Text("%.1f", static_cast<double>(total_time * in_ms));ImGui::NextColumn();
        ImGui::Text("%d", hits);ImGui::NextColumn();

        record.Clear();
    }
    ImGui::Columns(1);

    if(rs.selected == -1)
    {
        if(rs.records.empty() == false)
        {
            rs.selected = 0;
        }
    }

    if(rs.selected != -1)
    {
        const auto& selected = rs.records[Cint_to_sizet(rs.selected)];
        const auto& v = selected.history_of_total_times;
        if(v.size() >= 2)
        {
            ImGui::PlotLines
            (
                "Frame Times",
                v.data(),
                Csizet_to_int(v.size()),
                0,
                nullptr,
                -1.0f/in_ms,
                30.0f/in_ms,
                ImVec2{0.0f, 60.0f}
            );
        }
    }

    if(rs.running == false)
    {
        if(rs.selected_history == -1)
        {
            rs.selected_history = 0;
        }
        auto max = rs.records.empty() ? 10 : Csizet_to_int(rs.records[0].history_of_total_times.size()-1);

        // todo(Gustav): find a way to click in the ImGui::PlotLines to select the frame
        ImGui::SliderInt("History", &rs.selected_history, 0, max);
        ImGui::InputInt("History detail", &rs.selected_history);
    }

    ImGui::End();
}
