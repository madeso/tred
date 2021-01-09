#pragma once


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

    InputActionMap actions;
    PlayerHandleVector players;
    MappingList configs;
    InputDirector input_director;
};

}

