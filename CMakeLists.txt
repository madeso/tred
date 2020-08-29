cmake_minimum_required(VERSION 3.15)
set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
project(tred C CXX)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(cmake/StandardProjectSettings.cmake)
include(cmake/CompilerWarnings.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/compress.cmake)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

target_compile_features(project_options INTERFACE cxx_std_17)
set_project_warnings(project_warnings)
enable_sanitizers(project_options)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_ENTITY_DEBUG_COMPONENT "when converting components, (debug) assert of bad casts" OFF)
set(BUILD_TYPEINFO_IMPL "rtti" CACHE STRING "rtti, ctti, custom_hashed, custom_rt")

set(OpenGL_GL_PREFERENCE "GLVND")

set(BUILD_TYPEINFO_IMPL_RTTI FALSE)
set(BUILD_TYPEINFO_IMPL_CTTI FALSE)
set(BUILD_TYPEINFO_IMPL_CUSTOM_HASHED FALSE)
set(BUILD_TYPEINFO_IMPL_CUSTOM_RT FALSE)

message(STATUS "BUILD_TYPEINFO_IMPL: ${BUILD_TYPEINFO_IMPL}")
if(BUILD_TYPEINFO_IMPL STREQUAL "rtti")
    set(BUILD_TYPEINFO_IMPL_RTTI TRUE)
elseif(BUILD_TYPEINFO_IMPL STREQUAL "ctti")
    set(BUILD_TYPEINFO_IMPL_CTTI TRUE)
elseif(BUILD_TYPEINFO_IMPL STREQUAL "custom_hashed")
    set(BUILD_TYPEINFO_IMPL_CUSTOM_HASHED TRUE)
elseif(BUILD_TYPEINFO_IMPL STREQUAL "custom_rt")
    set(BUILD_TYPEINFO_IMPL_CUSTOM_RT TRUE)
else()
    message(STATUS "Invalid build status: ${BUILD_TYPEINFO_IMPL}")
endif()



find_package(OpenGL REQUIRED)
# find_package(assimp REQUIRED)
find_package(SDL2 REQUIRED)

set(MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/osx-bundle.plist")

# compiler flags
if(${MSVC})
    add_definitions(/D _CRT_SECURE_NO_WARNINGS)

    add_definitions(/D NOMINMAX)
    add_definitions(/D STRICT)
    add_definitions(/D WIN32_LEAN_AND_MEAN)
    add_definitions(/D VC_EXTRALEAN)

    # character set to Not Set=ASCII/SBCS
    # http://public.kitware.com/Bug/view.php?id=12189
    add_definitions(/D _SBCS)
endif()

# disable rtti

if(NOT BUILD_TYPEINFO_IMPL_RTTI)
    if(MSVC)
        add_compile_options(/GR-)
    else()
        add_compile_options(-fno-rtti)
    endif()
endif()

# enable all warnings
if(MSVC)
    add_compile_options(/W4)
endif()


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

if(BUILD_TYPEINFO_IMPL_CTTI)
    include_directories(SYSTEM external/ctti/include)
endif()

# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_in.h ${CMAKE_CURRENT_BINARY_DIR}/euph_generated_config.h)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})


set(src
    main.cc
    debug_opengl.cc debug_opengl.h
    stb.cc
    
    external/glad/src/glad.c
    external/glad/include/glad/glad.h
)
source_group("" FILES ${src})

message(STATUS "Sdl include ${SDL2_INCLUDE_DIR}")
message(STATUS "Sdl lib ${SDL2_LIBRARY}")

# imgui project dependency
add_library(imgui STATIC
    external/imgui-1.78/imgui_demo.cpp
    external/imgui-1.78/imgui_widgets.cpp
    external/imgui-1.78/imgui.cpp
    external/imgui-1.78/imgui_draw.cpp
    external/imgui-1.78/examples/imgui_impl_sdl.cpp
    external/imgui-1.78/examples/imgui_impl_opengl3.cpp
)
target_include_directories(imgui
    PUBLIC
    external/imgui-1.78/
    external/imgui-1.78/examples/
    PRIVATE
    external/glad/include
    ${SDL2_INCLUDE_DIR}
)
target_link_libraries(
    imgui
    PUBLIC ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS}
)
target_compile_definitions(imgui PUBLIC -D IMGUI_IMPL_OPENGL_LOADER_GLAD)


# imgui
add_executable(compress compress.cpp)

# custom
add_executable(embed embed.cc)
target_link_libraries(embed PRIVATE project_options project_warnings)

embed(HEADERS embedded_headers
    FILES
    vertex.glsl fragment.glsl
    light_vertex.glsl light_fragment.glsl
)
compress(HEADERS compressed_headers
    FILES container.jpg awesomeface.png
)
set(src ${src} ${embedded_headers} ${compressed_headers})

add_executable(tred WIN32 MACOSX_BUNDLE ${src})
target_include_directories(tred SYSTEM PRIVATE
    ${SDL2_INCLUDE_DIR}
    external/glad/include
    external/glm-0.9.9.8
    external/stb
)
target_link_libraries(
    tred
    PUBLIC ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${CMAKE_DL_LIBS} imgui
    PRIVATE project_options project_warnings
)

