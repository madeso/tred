set(src
    tred/debug_opengl.cc tred/debug_opengl.h
    tred/profiler.cc tred/profiler.h
    tred/cint.cc tred/cint.h
)

add_library(tred STATIC ${src})
target_link_libraries(tred
    PUBLIC sdl2 opengl imgui fmt::fmt stb glm
    PRIVATE project_options project_warnings
)
target_include_directories(tred
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

source_group("" FILES ${src})