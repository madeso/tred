#include "tred/input-keyconfig.h"

#include <cassert>

#include "tred/input-unitdef.h"
#include "tred/input-bindmap.h"
#include "tred/input-activelist.h"
#include "tred/input-director.h"


namespace input
{

KeyConfig::KeyConfig()
{
}


void KeyConfig::Add(std::shared_ptr<UnitDef> def)
{
    assert(def);
    definitions_.push_back(def);
}


std::shared_ptr<ConnectedUnits> KeyConfig::Connect(const InputActionMap& actions, InputDirector* director)
{
    assert(director);
    std::shared_ptr<ActiveList> actives(new ActiveList());
    binds_.reset(new BindMap(actions, actives.get()));

    std::shared_ptr<ConnectedUnits> units(new ConnectedUnits(actives));
    for (auto def: definitions_)
    {
        auto unit = def->Create(director, binds_.get());
        assert(unit);
        units->Add(unit);
    }
    return units;
}


}  // namespace input
