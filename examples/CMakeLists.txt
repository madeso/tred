set(src
    main.cc
    assets.cc assets.h
)

embed(embedded
    AS_TEXT
        ../data/shader_material.glsl
        ../data/shader_light.glsl
    AS_BINARY
        ../data/container_diffuse.png
        ../data/container_specular.png
)

add_executable(example WIN32 MACOSX_BUNDLE ${src} ${embedded})
target_link_libraries(
    example
    PUBLIC tred sprites_cards
    PRIVATE project_options project_warnings
)
target_include_directories(example
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

source_group("" FILES ${src})
source_group("generated" FILES ${generated})

