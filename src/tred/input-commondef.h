#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

#include "tred/input-binddef.h"
#include "tred/input-bind.h"
#include "tred/input-bindmap.h"


namespace input
{
struct InputActionMap;
struct BindMap;

struct CommonDef
{
    std::string bindname;
    std::string actionname;
    std::string type;
};

struct BindData
{
    std::shared_ptr<Bind> bind;
    bool invert;
    float scale;
};

void TransformAndSetBindValue(const BindData& data, float value);

template <typename TBind, typename Type>
std::vector<std::shared_ptr<TBind>> CreateBinds(std::vector<BindDef<Type>> defs, BindMap* map)
{
    std::vector<std::shared_ptr<TBind>> keybinds;
    for (const auto& key: defs)
    {
        std::shared_ptr<TBind> b(new TBind(key.type(), map->GetBindByName(key.id()), key.invert(), key.scale()));
        keybinds.push_back(b);
    }
    return keybinds;
}


template <typename TBind, typename Type>
std::map<Type, BindData> ConvertToBindMap(
        const std::vector<std::shared_ptr<TBind>>& axis)
{
    std::map<Type, BindData> actions_;
    for (auto a: axis)
    {
        BindData d;
        d.bind = a->bind();
        d.invert = a->invert();
        d.scale = a->scale();
        actions_.insert(std::make_pair(a->GetType(), d));
    }
    return actions_;
}


}  // namespace input
