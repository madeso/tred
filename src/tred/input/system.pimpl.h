#pragma once


#include "tred/input/director.h"
#include "tred/input/mappinglist.h"
#include "tred/input/actionmap.h"

#include "tred/handle.h"


namespace input
{


struct InputSystemPiml
{
    using PlayerHandleFunctions = handle_functions64<PlayerHandle>;
    using PlayerHandleVector = handle_vector<std::unique_ptr<player>, PlayerHandleFunctions>;

    input_action_map actions;
    PlayerHandleVector players;
    mapping_list configs;
    input::input_director input_director;
};

}

