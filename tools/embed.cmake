#
# This module exposes the following functions:
#
#  - embed
#  - embed_name
#
# Usage:
#
# embed(files
#   AS_TEXT readme.md
#   AS_BINARY image.png another_image.png
# )
# add_executable(thing ${files} main.cc)
#
# embed(variable_where_we_save_all_headers
#   SOURCE_GROUP optional_source_group_name_or_it_will_default_to_Generated
#   AS_TEXT zero_or_more_files_to_embed_as_text
#   AS_BINARY zero_or_more_files_to_embed_as_uncompressed_binary
#   AS_COMPRESSED zero_or_more_files_to_embed_as_combpressed_binary
# )
#
# By default the headerfile fill be called readme.md.h and the c++ constant will be README_MD
# use embed_name(file name) to change the base.
#
# embed_name(readme.md notes)
# include file is now notes.h and variable is NOTES
#
#
function(embed_name file name)
    set_source_files_properties(${file} PROPERTIES prop_embed_name ${name})
endfunction()


function(embed_internal_helper)
    get_source_file_property(input_name ${input_file} prop_embed_name)
    if(NOT input_name)
        get_filename_component(input_name ${input_file} NAME)
    endif()
    get_filename_component(absolute_file ${input_file} ABSOLUTE)

    set(input_variable ${input_name})
    string(TOUPPER "${input_variable}" input_variable)
    string(REPLACE "." "_" input_variable "${input_variable}")

    # return values => set in parent scope
    set(input_name ${input_name} PARENT_SCOPE)
    set(absolute_file ${absolute_file} PARENT_SCOPE)
    set(input_variable ${input_variable} PARENT_SCOPE)

    # append to a referenc to a list => set variable in parent scope based on inhertied value from parent scope
    set(headers_lists ${headers_lists} ${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h PARENT_SCOPE)
endfunction()


function(embed HEADERS_TARGET)
    set(options)
    set(oneValueArgs SOURCE_GROUP)
    set(multiValueArgs AS_TEXT AS_BINARY AS_COMPRESSED)
    cmake_parse_arguments(PARSE_ARGV 1 ARG
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
    )

    set(headers_lists)
    
    foreach(input_file ${ARG_AS_TEXT})
        embed_internal_helper()
        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h"
            MAIN_DEPENDENCY ${absolute_file}
            COMMAND embed_text
            ARGS "${absolute_file}" "${input_variable}" > "${input_name}.h"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Embedding text ${input_variable} to ${input_name}.h"
        )
    endforeach()

    foreach(input_file ${ARG_AS_BINARY})
        embed_internal_helper()
        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h"
            MAIN_DEPENDENCY ${absolute_file}
            COMMAND embed_binary
            ARGS "-nocompress" "${absolute_file}" "${input_variable}" > "${input_name}.h"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Embedding binary ${input_variable} to ${input_name}.h"
        )
    endforeach()

    foreach(input_file ${ARG_AS_COMPRESSED})
        embed_internal_helper()
        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h"
            MAIN_DEPENDENCY ${absolute_file}
            COMMAND embed_binary
            ARGS "${absolute_file}" "${input_variable}" > "${input_name}.h"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Compressing binary ${input_variable} to ${input_name}.h"
        )
    endforeach()

    source_group("${ARG_SOURCE_GROUP}" FILES ${headers_lists})
    set(${HEADERS_TARGET} ${headers_lists} PARENT_SCOPE)
endfunction()
