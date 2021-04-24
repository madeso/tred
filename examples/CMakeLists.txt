set(src
    main.cc
)

embed(embedded
    AS_TEXT
        ../data/shader_vertex.glsl ../data/shader_fragment.glsl
        ../data/light_vertex.glsl ../data/light_fragment.glsl
    AS_BINARY
        ../data/container_diffuse.png
        ../data/container_specular.png
)

add_executable(example WIN32 MACOSX_BUNDLE ${src} ${embedded})
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

embed(demo_cards
    AS_BINARY
        ../data/sprites/cards.png
)
add_executable(demo WIN32 MACOSX_BUNDLE demo.cc ${demo_cards})
target_link_libraries(
    demo
    PUBLIC tred sprites
    PRIVATE project_options project_warnings
)
target_include_directories(demo
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)
