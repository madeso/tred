#pragma once

#include <memory>

#include "tred/result.h"

#include "tred/input/config.h"


namespace input
{


struct Mapping;
struct ActionMap;


struct MappingList
{
    MappingList();
    MappingList(MappingList&& m);
    void operator=(MappingList&& m);
    ~MappingList();

    void add(const std::string& name, std::unique_ptr<Mapping>&& config);

    std::unordered_map<std::string, std::unique_ptr<Mapping>> configs;
};


Result<MappingList> load_mapping_list(const input::config::MappingList& root, const ActionMap& map);

}
