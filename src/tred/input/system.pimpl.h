#pragma once


#include "tred/input/player.h"
#include "tred/input/system.h"
#include "tred/input/director.h"
#include "tred/input/mappinglist.h"
#include "tred/input/actionmap.h"

#include "tred/handle.h"


namespace input
{


struct InputSystemPiml
{
    using PlayerHandleFunctions = HandleFunctions64<PlayerHandle>;
    using PlayerHandleVector = HandleVector<std::unique_ptr<Player>, PlayerHandleFunctions>;

    ActionMap actions;
    PlayerHandleVector players;
    MappingList configs;
    input::Director input_director;
};

}

