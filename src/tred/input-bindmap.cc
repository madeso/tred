#include "tred/input-bindmap.h"

#include <cassert>
#include "fmt/format.h"

#include "tred/input-action.h"
#include "tred/input-range.h"
#include "tred/input-activelist.h"
#include "tred/input-activemasteraxis.h"
#include "tred/input-activemasterrange.h"
#include "tred/input-activerangetoaxis.h"
#include "tred/input-activeaxistorange.h"


namespace input
{


BindMap::BindMap(const InputActionMap& actions, ActiveList* actives)
{
    const auto as = actions.GetActionList();

    for (auto action: as)
    {
        switch (action->range)
        {
        case Range::INFINITE:
        case Range::WITHIN_NEGATIVE_ONE_POSITIVE_ONE:
            AddAxis(action, actives);
            break;
        case Range::WITHIN_ZERO_ONE:
            AddRange(action, actives);
            break;
        default:
            const std::string error = fmt::format("Invalid range type {}", action->range);
            throw error;
        }
    }
}


std::shared_ptr<Bind> BindMap::GetBindByName(const std::string& name)
{
    auto res = binds.find(name);
    if (res == binds.end())
    {
        const std::string error = fmt::format("unable to find bind {}", name);
        throw error;
    }
    return res->second;
}


void BindMap::AddRange(std::shared_ptr<InputAction> action, ActiveList* actives)
{
    /// @todo verify that it is indeed a range

    std::shared_ptr<Bind> bind(new Bind(action.get(), BindType::RANGE));
    std::shared_ptr<ActiveRange> active(new ActiveRange(action.get(), bind.get()));
    actives->Add(active);
    binds.insert(std::make_pair(action->name, bind));

    bind.reset(new Bind(action.get(), BindType::AXIS));
    std::shared_ptr<ActiveAxisToRange> activeAxis(new ActiveAxisToRange(action.get(), bind.get()));
    actives->Add(activeAxis);
    binds.insert(std::make_pair(action->name + "-axis", bind));

    std::shared_ptr<ActiveMasterRange> masterRange(new ActiveMasterRange(action.get(), active.get(), activeAxis.get()));
    actives->Add(masterRange);
}


void BindMap::AddAxis(std::shared_ptr<InputAction> action, ActiveList* actives)
{
    /// @todo verify that it is indeed a axis

    std::shared_ptr<Bind> bind(new Bind(action.get(), BindType::AXIS));
    std::shared_ptr<ActiveAxis> active(new ActiveAxis(action.get(), bind.get()));
    actives->Add(active);
    binds.insert(std::make_pair(action->name, bind));

    std::shared_ptr<Bind> bindNeg(new Bind(action.get(), BindType::RANGE));
    std::shared_ptr<Bind> bindPos(new Bind(action.get(), BindType::RANGE));
    std::shared_ptr<ActiveRangeToAxis> activeAxis(new ActiveRangeToAxis(action.get(), bindPos.get(), bindNeg.get()));
    actives->Add(activeAxis);
    binds.insert(std::make_pair(action->name + "-", bindNeg));
    binds.insert(std::make_pair(action->name + "+", bindPos));

    std::shared_ptr<ActiveMasterAxis> masterAxis(new ActiveMasterAxis(action.get(), active.get(), activeAxis.get()));
    actives->Add(masterAxis);
}


}  // namespace input