#pragma once

#include <map>
#include <string>
#include <memory>

#include "tred/input/config.h"


namespace input
{


struct Mapping;
struct InputActionMap;


struct MappingList
{
    MappingList();
    ~MappingList();

    void Add(const std::string& name, std::unique_ptr<Mapping>&& config);

    std::map<std::string, std::unique_ptr<Mapping>> configs;
};


void Load(MappingList* configs, const input::config::MappingList& root, const InputActionMap& map);

}  // namespace input
