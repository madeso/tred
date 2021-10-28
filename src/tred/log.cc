#include "tred/log.h"

#include "tred/sdl.h"


void
log_info_implementation(fmt::string_view format, fmt::format_args args)
{
    const auto text = fmt::vformat(format, args);
    SDL_Log("%s", text.c_str());
}


void
log_error_implementation(fmt::string_view format, fmt::format_args args)
{
    const auto text = fmt::vformat(format, args);
    SDL_Log("%s", text.c_str());
}


void
log_warning_implementation(fmt::string_view format, fmt::format_args args)
{
    const auto text = fmt::vformat(format, args);
    SDL_Log("%s", text.c_str());
}

