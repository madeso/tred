include (embed.cmake)

add_executable(embed_binary embed_binary.cpp)
target_link_libraries(embed_binary
    PRIVATE
        project_options
        imgui_binary_to_compressed
)


add_executable(embed_text embed_text.cc)
target_link_libraries(embed_text
    PRIVATE
        project_options
        project_warnings
)
