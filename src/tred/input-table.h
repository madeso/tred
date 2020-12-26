#pragma once

#include <string>
#include <map>

struct Table
{
public:
    void Set(const std::string& name, float value);
    
    std::map<std::string, float> data;
};

