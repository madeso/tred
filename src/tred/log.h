#pragma once

#include <fmt/format.h>


void
LogInfoImpl(fmt::string_view format, fmt::format_args args);

void
LogErrorImpl(fmt::string_view format, fmt::format_args args);


template <typename S, typename... Args>
void
LogInfo(const S& format, Args&&... args) {
    LogInfoImpl(format, fmt::make_args_checked<Args...>(format, args...));
}

template <typename S, typename... Args>
void
LogError(const S& format, Args&&... args) {
    LogErrorImpl(format, fmt::make_args_checked<Args...>(format, args...));
}

// todo(Gustav): fix va args expansion with no arguments

#if 0
    #define LOG_INFO(format, ...) LogInfo(FMT_STRING(format), ##__VA_ARGS__)
    #define LOG_ERROR(format, ...) LogError(FMT_STRING(format), ##__VA_ARGS__)
#else
    #define LOG_INFO(...) LogInfo(__VA_ARGS__)
    #define LOG_ERROR(...) LogError(__VA_ARGS__)
#endif
