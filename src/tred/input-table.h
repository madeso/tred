#pragma once

#include <string>
#include <map>

class Table
{
public:
    void Set(const std::string& name, float value);
    
    std::map<std::string, float> data;
};

