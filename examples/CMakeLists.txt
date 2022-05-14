set(src
    main.cc
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
    PUBLIC tred sprites_cards mustache
    PRIVATE project_options project_warnings
)
target_include_directories(example
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

source_group("" FILES ${src})
source_group("generated" FILES ${generated})



embed(demo_letter
    AS_BINARY
        ../data/sprites/letter_g.png
)
add_executable(demo WIN32 MACOSX_BUNDLE demo.cc ${demo_letter})
target_link_libraries(
    demo
    PUBLIC tred sprites_cards
    PRIVATE project_options project_warnings
)
target_include_directories(demo
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)



add_executable(minesweeper WIN32 MACOSX_BUNDLE minesweeper.cc)
target_link_libraries(
    minesweeper
    PUBLIC tred sprites_onebit
    PRIVATE project_options project_warnings
)
target_include_directories(minesweeper
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(4d WIN32 MACOSX_BUNDLE 4thdimension.cc)
target_link_libraries(
    4d
    PUBLIC tred sprites_onebit
    PRIVATE project_options project_warnings
)
