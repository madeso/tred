function(compress input_file file_list dest_files input_name)
    string(TOUPPER ${input_name} input_name_upper)
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h"
        COMMAND compress
        ARGS ${input_file} ${input_name_upper} > ${input_name}.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating ${input_name}"
    )
    set(file_list ${file_list} ${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h)
    set(${dest_files} ${file_list} PARENT_SCOPE)
    source_group("generatedd" FILES ${CMAKE_CURRENT_BINARY_DIR}/${input_name}.h)
endfunction()
