set(src
    sprites/onebit.cc sprites/onebit.h
)
embed(sprites
    AS_BINARY
        onebit.png
)

add_library(sprites_onebit STATIC ${src} ${sprites})
target_link_libraries(sprites_onebit
    PUBLIC tred
    PRIVATE project_options project_warnings fmt
)
target_include_directories(sprites_onebit
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)
