#include "tred/input/mappinglist.h"

#include <cassert>
#include <stdexcept>
#include <fstream>
#include "fmt/format.h"


#include "tred/input/mapping.h"
#include "tred/input/keyboarddef.h"
#include "tred/input/joystickdef.h"
#include "tred/input/mousedef.h"
#include "tred/input/config.h"


namespace input
{


MappingList::MappingList()
{
}

MappingList::~MappingList()
{
}


void MappingList::Add(const std::string& name, std::unique_ptr<Mapping>&& config)
{
    assert(config);
    configs.insert(std::make_pair(name, std::move(config)));
}


void Load(Mapping* config, const input::config::Mapping& root)
{
    assert(config);

    for (const auto& d: root.keyboards)
    {
        config->Add(std::make_unique<KeyboardDef>(d, &config->converter));
    }

    for (const auto& d: root.mouses)
    {
        config->Add(std::make_unique<MouseDef>(d, &config->converter));
    }

    int joystick_id = 0;
    for (const auto& d: root.joysticks)
    {
        config->Add(std::make_unique<JoystickDef>(joystick_id, d, &config->converter));
        joystick_id += 1;
    }
}


void Load(MappingList* configs, const input::config::MappingList& root, const InputActionMap& map)
{
    assert(configs);

    for (const auto& d: root)
    {
        const std::string name = d.name;
        auto config = std::make_unique<Mapping>(map, d);
        Load(config.get(), d);
        configs->Add(name, std::move(config));
    }
}


}  // namespace input
