#include "tred/profiler.h"

#include <vector>
#include <numeric>

#include "imgui.h"

#include "tred/cint.h"

#include "fmt/format.h"

namespace
{
    struct profile_record
    {
        std::vector<float> times;

        void add(float total_time)
        {
            constexpr std::size_t MAX_SIZE = 100;

            if(times.size() >= MAX_SIZE) { times.erase(times.begin()); }
            times.push_back(total_time);
        }
    };

    profile_record& get_records()
    {
        static auto records = profile_record{};
        return records;
    }

    void add_record(float time_taken)
    {
        auto& r = get_records();
        r.add(time_taken);
    }
}


frame_profile_scope::frame_profile_scope()
    : start(clock::now())
{
}


frame_profile_scope::~frame_profile_scope()
{
    const auto end = clock::now();
    std::chrono::duration<float> time_taken = end-start;
    add_record(time_taken.count());
}


namespace
{
    constexpr float in_ms = 1000.0f;
}

void imgui_text(const std::string& str)
{
    ImGui::Text("%s", str.c_str());
}

void
PrintTimes(float time)
{
    imgui_text(fmt::format("Frametime {}", time * in_ms));
    imgui_text(fmt::format("FPS %.1f", 1.0f/time));
}


void
print_profiles()
{
    if(ImGui::Begin("Profiler") == false) { ImGui::End(); return; }

    auto& record = get_records();

    if(record.times.empty() == false)
    {
        float time = *record.times.rbegin();

        PrintTimes(time);
    }
    
    {
        if(record.times.size() >= 2)
        {
            ImGui::PlotLines
            (
                "",
                record.times.data(),
                Csizet_to_int(record.times.size()),
                0,
                nullptr,
                -1.0f/in_ms,
                30.0f/in_ms,
                ImVec2{0.0f, 60.0f}
            );

            const float sum = std::accumulate(record.times.begin(), record.times.end(), 0.0f);
            const float average = sum / static_cast<float>(record.times.size());
            PrintTimes(average);
        }
    }

    ImGui::End();
}
