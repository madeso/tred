add_subdirectory(fmt-7.1.1)

###################################################################################################
# glad
add_library(glad STATIC
    glad/src/glad.c
    glad/include/glad/glad.h
)
target_include_directories(glad
    PUBLIC
        glad/include
)


###################################################################################################
# imgui
add_library(imgui STATIC
    imgui-1.78/imgui_demo.cpp
    imgui-1.78/imgui_widgets.cpp
    imgui-1.78/imgui.cpp
    imgui-1.78/imgui_draw.cpp
    imgui-1.78/examples/imgui_impl_sdl.cpp
    imgui-1.78/examples/imgui_impl_opengl3.cpp
)
target_include_directories(imgui
    PUBLIC
        imgui-1.78/
        imgui-1.78/examples/
)
target_link_libraries(imgui
    PUBLIC
        glad
        opengl
        sdl2
)
target_compile_definitions(imgui
    PUBLIC
        IMGUI_IMPL_OPENGL_LOADER_GLAD
)


###################################################################################################
# SDL2
find_package(SDL2 REQUIRED)
add_library(sdl2 INTERFACE)
target_include_directories(sdl2
    INTERFACE
        ${SDL2_INCLUDE_DIR}
)
target_link_libraries(sdl2
    INTERFACE
        ${SDL2_LIBRARY}
        ${CMAKE_DL_LIBS}
)


###################################################################################################
# OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
add_library(opengl INTERFACE)
target_link_libraries(opengl INTERFACE OpenGL::GL)


###################################################################################################
# glm
add_library(glm INTERFACE)
target_include_directories(glm
    INTERFACE
        glm-0.9.9.8
)


###################################################################################################
# catch
add_library(catch INTERFACE)
target_include_directories(catch
    INTERFACE
        catch
)


###################################################################################################
# stb
add_library(stb STATIC
    stb.cc
)
target_include_directories(stb
    PUBLIC
        stb
)
