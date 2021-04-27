#pragma once

#include <string>
#include <map>


namespace input
{


struct table
{
    void set(const std::string& name, float value);

    std::map<std::string, float> data;
};


}

