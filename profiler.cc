#include "profiler.h"

#include <vector>
#include <numeric>

#include "imgui.h"

#include "cint.h"


namespace
{
    struct ProfileRecord
    {
        std::vector<float> times;

        void Add(float total_time)
        {
            constexpr std::size_t MAX_SIZE = 100;

            if(times.size() >= MAX_SIZE) { times.erase(times.begin()); }
            times.push_back(total_time);
        }
    };

    ProfileRecord& GetRecords()
    {
        static auto records = ProfileRecord{};
        return records;
    }

    void AddRecord(float time_taken)
    {
        auto& r = GetRecords();
        r.Add(time_taken);
    }
}


FrameProfileScope::FrameProfileScope()
    : start(Clock::now())
{
}


FrameProfileScope::~FrameProfileScope()
{
    const auto end = Clock::now();
    std::chrono::duration<float> time_taken = end-start;
    AddRecord(time_taken.count());
}


namespace
{
    constexpr float in_ms = 1000.0f;
}


void
PrintTimes(float time)
{
    ImGui::Text("Frametime %.1f", static_cast<double>(time * in_ms));
    ImGui::Text("FPS %.1f", static_cast<double>(1/(time)));
}


void
PrintProfiles()
{
    if(ImGui::Begin("Profiler") == false) { ImGui::End(); return; }

    auto& record = GetRecords();

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
