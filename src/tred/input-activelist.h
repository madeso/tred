#pragma once

#include <vector>
#include <memory>


struct Table;

namespace input
{
struct ActiveRange;
struct ActiveAxis;

struct ActiveAxisToRange;
struct ActiveRangeToAxis;

struct ActiveMasterAxis;
struct ActiveMasterRange;

struct ActiveList
{
    void Add(std::shared_ptr<ActiveRange> range);
    void Add(std::shared_ptr<ActiveAxis> axis);

    void Add(std::shared_ptr<ActiveAxisToRange> axis);
    void Add(std::shared_ptr<ActiveRangeToAxis> axis);

    void Add(std::shared_ptr<ActiveMasterAxis> axis);
    void Add(std::shared_ptr<ActiveMasterRange> axis);

    void UpdateTable(Table* table);

    /// @todo move to a global list
    void Update(float dt);

    std::vector<std::shared_ptr<ActiveRange>> range_binds_;
    std::vector<std::shared_ptr<ActiveAxis>> axis_binds_;
    std::vector<std::shared_ptr<ActiveAxisToRange>> axis_to_range_binds_;
    std::vector<std::shared_ptr<ActiveRangeToAxis>> range_to_axis_binds_;
    std::vector<std::shared_ptr<ActiveMasterAxis>> master_axis_binds_;
    std::vector<std::shared_ptr<ActiveMasterRange>> master_range_binds_;
};


}  // namespace input
