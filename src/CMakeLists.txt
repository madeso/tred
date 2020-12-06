set(src
    tred/cint.cc tred/cint.h
    tred/handle.cc tred/handle.h
    tred/image.cc tred/image.h
    tred/log.cc tred/log.h
    tred/mesh.cc tred/mesh.h
    tred/mesh.compiled.cc tred/mesh.compiled.h
    tred/mesh.default.cc tred/mesh.default.h
    tred/opengl.cc tred/opengl.h
    tred/opengl.debug.cc tred/opengl.debug.h
    tred/profiler.cc tred/profiler.h
    tred/shader.cc tred/shader.h
    tred/shader.util.cc tred/shader.util.h
    tred/texture.cc tred/texture.h
    tred/types.cc tred/types.h
    tred/uniform.cc tred/uniform.h
    tred/vertex_layout.cc tred/vertex_layout.h
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

set(src_test
    tred/handle.test.cc
    ../external/catch/main.cc
)
add_executable(test ${src_test})
target_link_libraries(test
    PUBLIC catch tred
    PRIVATE project_options project_warnings
)
