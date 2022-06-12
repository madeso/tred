#pragma once


#include <map>


namespace input
{


struct Table
{
    void set(const std::string& name, float value);

    std::map<std::string, float> data;
};


}

