include(compress.cmake)

# imgui
add_executable(compress compress.cpp)

# custom
add_executable(embed embed.cc)
target_link_libraries(embed PRIVATE project_options project_warnings)
