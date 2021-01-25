#pragma once

#include <map>
#include <string>
#include <memory>

#include "tred/result.h"

#include "tred/input/config.h"


namespace input
{


struct Mapping;
struct InputActionMap;


struct MappingList
{
    MappingList();
    MappingList(MappingList&& m);
    void operator=(MappingList&& m);
    ~MappingList();

    void Add(const std::string& name, std::unique_ptr<Mapping>&& config);

    std::map<std::string, std::unique_ptr<Mapping>> configs;
};


Result<MappingList> LoadMappingList(const input::config::MappingList& root, const InputActionMap& map);

}  // namespace input
