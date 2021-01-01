#include "tred/log.h"

#include "SDL.h"


void
LogInfoImpl(fmt::string_view format, fmt::format_args args)
{
    const auto text = fmt::vformat(format, args);
    SDL_Log("%s", text.c_str());
}


void
LogErrorImpl(fmt::string_view format, fmt::format_args args)
{
    const auto text = fmt::vformat(format, args);
    SDL_Log("%s", text.c_str());
}


void
LogWarningImpl(fmt::string_view format, fmt::format_args args)
{
    const auto text = fmt::vformat(format, args);
    SDL_Log("%s", text.c_str());
}

