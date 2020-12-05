set(src
    main.cc
)

embed(HEADERS embedded_source
    FILES
    ../data/shader_vertex.glsl ../data/shader_fragment.glsl
    ../data/light_vertex.glsl ../data/light_fragment.glsl
)
compress(HEADERS compressed_source
    FILES ../data/container_diffuse.png ../data/container_specular.png
)
set(generated ${embedded_source} ${compressed_source})

add_executable(example WIN32 MACOSX_BUNDLE ${src} ${generated})
target_link_libraries(
    example
    PUBLIC tred
    PRIVATE project_options project_warnings
)
target_include_directories(example
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

source_group("" FILES ${src})
source_group("generated" FILES ${generated})
