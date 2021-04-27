#pragma once

#include <map>
#include <string>
#include <memory>

#include "tred/result.h"

#include "tred/input/config.h"


namespace input
{


struct mapping;
struct input_action_map;


struct mapping_list
{
    mapping_list();
    mapping_list(mapping_list&& m);
    void operator=(mapping_list&& m);
    ~mapping_list();

    void add(const std::string& name, std::unique_ptr<mapping>&& config);

    std::map<std::string, std::unique_ptr<mapping>> configs;
};


result<mapping_list> load_mapping_list(const input::config::mapping_list& root, const input_action_map& map);

}
