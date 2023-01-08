cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
project(tred C CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

add_library(edit_and_continue INTERFACE)
if(MSVC)
    # ZI = add debug information with edit and continue support to the target
    # Zi = same with edit and continue disabled
    target_compile_options(edit_and_continue INTERFACE "/ZI")
    target_link_options(edit_and_continue INTERFACE "/INCREMENTAL")
endif()

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

target_link_libraries(project_options INTERFACE edit_and_continue)

target_compile_features(project_options INTERFACE cxx_std_17)
set_project_warnings(project_warnings)
enable_sanitizers(project_options)

add_subdirectory(external)
add_subdirectory(data/sprites)
add_subdirectory(examples)

add_library(sprites INTERFACE)
target_include_directories(sprites INTERFACE data)



# -----------------------------------------------------------------------------
# tred

set(src_input
    src/tred/input/action.cc src/tred/input/action.h
    src/tred/input/actionmap.cc src/tred/input/actionmap.h
    src/tred/input/activeunit.cc src/tred/input/activeunit.h
    src/tred/input/activeunit.gamecontroller.cc src/tred/input/activeunit.gamecontroller.h
    src/tred/input/activeunit.joystick.cc src/tred/input/activeunit.joystick.h
    src/tred/input/activeunit.keyboard.cc src/tred/input/activeunit.keyboard.h
    src/tred/input/activeunit.mouse.cc src/tred/input/activeunit.mouse.h
    src/tred/input/axis.cc src/tred/input/axis.h
    src/tred/input/axistype.cc src/tred/input/axistype.h
    src/tred/input/bind.cc src/tred/input/bind.h
    src/tred/input/bindunit.cc src/tred/input/bindunit.h
    src/tred/input/config.cc src/tred/input/config.h
    src/tred/input/connectedunits.cc src/tred/input/connectedunits.h
    src/tred/input/director.cc src/tred/input/director.h
    src/tred/input/hataxis.cc src/tred/input/hataxis.h
    src/tred/input/index.cc src/tred/input/index.h
    src/tred/input/key.cc src/tred/input/key.h
    src/tred/input/mapping.cc src/tred/input/mapping.h
    src/tred/input/mapping.detection.cc src/tred/input/mapping.detection.h
    src/tred/input/mappinglist.cc src/tred/input/mappinglist.h
    src/tred/input/platform.cc src/tred/input/platform.h
    src/tred/input/player.cc src/tred/input/player.h
    src/tred/input/range.cc src/tred/input/range.h
    src/tred/input/smooth.cc src/tred/input/smooth.h
    src/tred/input/system.cc src/tred/input/system.h
    src/tred/input/system.pimpl.cc src/tred/input/system.pimpl.h
    src/tred/input/table.cc src/tred/input/table.h
    src/tred/input/unitdef.cc src/tred/input/unitdef.h
    src/tred/input/unitdef.gamecontroller.cc src/tred/input/unitdef.gamecontroller.h
    src/tred/input/unitdef.joystick.cc src/tred/input/unitdef.joystick.h
    src/tred/input/unitdef.keyboard.cc src/tred/input/unitdef.keyboard.h
    src/tred/input/unitdef.mouse.cc src/tred/input/unitdef.mouse.h
    src/tred/input/unitdiscovery.cc src/tred/input/unitdiscovery.h
    src/tred/input/unitsetup.cc src/tred/input/unitsetup.h
)

set(src_empty
    src/tred/render/empty/handles.cc

    src/tred/render/empty/light.cc
    src/tred/render/empty/render_world_interface.cc
    src/tred/render/empty/vfs.cc
)

set(src_render
    src/tred/render/handles.h
    src/tred/render/handle.funcs.h
    
    src/tred/render/light.h
    src/tred/render/render_world_interface.h
    src/tred/render/vfs.h

    src/tred/render/camera.cc src/tred/render/camera.h
    src/tred/render/compiled.mesh.cc src/tred/render/compiled.mesh.h
    src/tred/render/engine.cc src/tred/render/engine.h
    src/tred/render/material.property.cc src/tred/render/material.property.h
    src/tred/render/material.shader.source.cc src/tred/render/material.shader.source.h
    src/tred/render/mesh.cc src/tred/render/mesh.h
    src/tred/render/opengl_utils.cc src/tred/render/opengl_utils.h
    src/tred/render/render_func.cc src/tred/render/render_func.h
    src/tred/render/render2.cc src/tred/render/render2.h
    src/tred/render/shader.cc src/tred/render/shader.h
    src/tred/render/shader.source.cc src/tred/render/shader.source.h
    src/tred/render/texture.cc src/tred/render/texture.h
    src/tred/render/uniform.cc src/tred/render/uniform.h
    src/tred/render/vertex_layout.cc src/tred/render/vertex_layout.h
    src/tred/render/world.cc src/tred/render/world.h
    )
    
set(src_base
    src/tred/array.cc src/tred/array.h
    src/tred/assert.cc src/tred/assert.h
    src/tred/cint.cc src/tred/cint.h
    src/tred/colors.cc src/tred/colors.h
    src/tred/dependency_opengl.cc src/tred/dependency_opengl.h
    src/tred/dependency_sdl.cc src/tred/dependency_sdl.h src/tred/dependency_glad.h
    src/tred/handle.cc src/tred/handle.h
    src/tred/hash.cc src/tred/hash.h
    src/tred/hash.string.cc src/tred/hash.string.h
    src/tred/log.cc src/tred/log.h
    src/tred/random.cc src/tred/random.h
    src/tred/rect.cc src/tred/rect.h
    src/tred/result.cc src/tred/result.h
    src/tred/sortablepair.cc src/tred/sortablepair.h
    src/tred/template_engine.cc src/tred/template_engine.h
    src/tred/types.cc src/tred/types.h
    src/tred/windows.cc src/tred/windows.h
    src/tred/windows.sdl.convert.cc src/tred/windows.sdl.convert.h
    src/tred/windows.sdl.joystick.cc src/tred/windows.sdl.joystick.h
)

set(src_pch
    src/tred/pch.public.h src/tred/pch.private.h
)

set(src
    ${src_input}
    ${src_empty}
    ${src_render}
    ${src_base}
    ${src_pch}
)

add_library(tred STATIC ${src})
target_link_libraries(tred
    PUBLIC
        sdl2
        opengl
        imgui
        fmt::fmt
        stb::image
        glm
        embed
    PRIVATE
        project_options
        project_warnings
        mustache
)
target_include_directories(tred
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_precompile_headers(tred
    PUBLIC src/tred/pch.public.h
    PRIVATE src/tred/pch.private.h
)

source_group("" FILES ${src})

source_group("input" FILES ${src_input})
source_group("empty" FILES ${src_empty})
source_group("render" FILES ${src_render})
source_group("base" FILES ${src_base})
source_group("pch" FILES ${src_pch})


set(src_test
    src/tred/handle.test.cc
    src/tred/input/mapping.detection.test.cc
    src/tred/input/system.test.cc
    src/tred/render/shader.source.test.cc
    src/tred/render/vertex_layout.test.cc
    external/catch/main.cc
)
add_executable(test ${src_test})
target_link_libraries(test
    PUBLIC catch tred catchy
    PRIVATE project_options project_warnings
)


