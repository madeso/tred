#pragma once


namespace input
{


struct Table
{
    void set(const std::string& name, float value);

    std::unordered_map<std::string, float> data;
};


}

