set(src
    tred/dependency_sdl.cc tred/dependency_sdl.h tred/dependency_glad.h

    tred/assert.cc tred/assert.h

    tred/colors.cc tred/colors.h

    tred/cint.cc tred/cint.h
    tred/handle.cc tred/handle.h
    tred/texture.load.cc tred/texture.load.h
    tred/array.cc tred/array.h
    tred/texture.types.cc tred/texture.types.h

    tred/input/action.cc tred/input/action.h
    tred/input/actionmap.cc tred/input/actionmap.h
    tred/input/activeunit.cc tred/input/activeunit.h
    tred/input/axistype.cc tred/input/axistype.h
    tred/input/activeunit.gamecontroller.cc tred/input/activeunit.gamecontroller.h
    tred/input/activeunit.joystick.cc tred/input/activeunit.joystick.h
    tred/input/activeunit.keyboard.cc tred/input/activeunit.keyboard.h
    tred/input/activeunit.mouse.cc tred/input/activeunit.mouse.h
    tred/input/axis.cc tred/input/axis.h
    tred/input/bind.cc tred/input/bind.h
    tred/input/bindunit.cc tred/input/bindunit.h
    tred/input/config.cc tred/input/config.h
    tred/input/connectedunits.cc tred/input/connectedunits.h
    tred/input/director.cc tred/input/director.h
    tred/input/hataxis.cc tred/input/hataxis.h
    tred/input/index.cc tred/input/index.h
    tred/input/key.cc tred/input/key.h
    tred/input/mapping.cc tred/input/mapping.h
    tred/input/mapping.detection.cc tred/input/mapping.detection.h
    tred/input/mappinglist.cc tred/input/mappinglist.h
    tred/input/platform.cc tred/input/platform.h
    tred/input/player.cc tred/input/player.h
    tred/input/range.cc tred/input/range.h
    tred/input/smooth.cc tred/input/smooth.h
    tred/input/system.cc tred/input/system.h
    tred/input/table.cc tred/input/table.h
    tred/input/unitdef.cc tred/input/unitdef.h
    tred/input/unitdef.gamecontroller.cc tred/input/unitdef.gamecontroller.h
    tred/input/unitdef.joystick.cc tred/input/unitdef.joystick.h
    tred/input/unitdef.keyboard.cc tred/input/unitdef.keyboard.h
    tred/input/unitdef.mouse.cc tred/input/unitdef.mouse.h
    tred/input/unitdiscovery.cc tred/input/unitdiscovery.h
    tred/input/unitsetup.cc tred/input/unitsetup.h
    tred/input/system.pimpl.cc tred/input/system.pimpl.h

    tred/render/handle.texture.cc tred/render/handle.texture.h
    tred/render/handle.compiledmaterialshader.cc tred/render/handle.compiledmaterialshader.h
    tred/render/handle.world.cc tred/render/handle.world.h
    tred/render/handle.mesh.cc tred/render/handle.mesh.h

    tred/render/light.cc tred/render/light.h
    tred/render/light.uniforms.cc tred/render/light.uniforms.h
    tred/render/camera.cc tred/render/camera.h
    tred/render/camera.compiled.cc tred/render/camera.compiled.h
    tred/render/shader.template.cc tred/render/shader.template.h
    tred/render/material.property.cc tred/render/material.property.h
    tred/render/material.shader.source.cc tred/render/material.shader.source.h
    tred/render/material.cc tred/render/material.h
    tred/render/light.active.cc tred/render/light.active.h
    tred/render/light.params.cc tred/render/light.params.h
    tred/render/material.compiledprops.cc tred/render/material.compiledprops.h
    tred/render/compiledmaterialshader.cc tred/render/compiledmaterialshader.h
    tred/render/compiledmaterial.cc tred/render/compiledmaterial.h
    tred/render/mesh.cc tred/render/mesh.h
    tred/render/compiled.mesh.cc tred/render/compiled.mesh.h
    tred/render/vfs.cc tred/render/vfs.h
    tred/render/cache.cc tred/render/cache.h
    tred/render/renderlist.cc tred/render/renderlist.h
    tred/render/engine.cc tred/render/engine.h
    tred/render/scopedrenderer.cc tred/render/scopedrenderer.h
    tred/render/world.cc tred/render/world.h

    tred/render2.cc tred/render2.h
    tred/layer2.cc tred/layer2.h
    tred/spritebatch.cc tred/spritebatch.h
    tred/log.cc tred/log.h
    tred/geom.cc tred/geom.h
    tred/geom.compiled.cc tred/geom.compiled.h
    tred/geom.default.cc tred/geom.default.h
    tred/dependency_opengl.cc tred/dependency_opengl.h
    tred/opengl_utils.cc tred/opengl_utils.h
    tred/opengl_state.cc tred/opengl_state.h
    tred/opengl_debug.cc tred/opengl_debug.h
    tred/result.cc tred/result.h
    tred/shader.cc tred/shader.h
    tred/shader.source.cc tred/shader.source.h
    tred/shader.util.cc tred/shader.util.h
    tred/sortablepair.cc tred/sortablepair.h
    tred/texture.cc tred/texture.h
    tred/types.cc tred/types.h
    tred/uniform.cc tred/uniform.h
    tred/vertex_layout.cc tred/vertex_layout.h
    tred/windows.cc tred/windows.h
    tred/windows.sdl.joystick.cc tred/windows.sdl.joystick.h
    tred/windows.sdl.convert.cc tred/windows.sdl.convert.h
    tred/hash.cc tred/hash.h
    tred/hash.string.cc tred/hash.string.h
    tred/render_func.cc tred/render_func.h

    tred/fyro.h
    tred/game.cc tred/game.h
    tred/rect.cc tred/rect.h
    tred/viewportdef.cc tred/viewportdef.h

    tred/random.cc tred/random.h

    tred/pch.public.h tred/pch.private.h
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
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
target_precompile_headers(tred
    PUBLIC tred/pch.public.h
    PRIVATE tred/pch.private.h
)

source_group("" FILES ${src})

set(src_test
    tred/handle.test.cc
    tred/input/mapping.detection.test.cc
    tred/input/system.test.cc
    tred/shader.source.test.cc
    tred/vertex_layout.test.cc
    ../external/catch/main.cc
)
add_executable(test ${src_test})
target_link_libraries(test
    PUBLIC catch tred catchy
    PRIVATE project_options project_warnings
)
