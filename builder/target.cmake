function(group_sources)
    set(builder_dir ${BUILDER_SOURCE_DIR})
    set(binary_dir  ${CMAKE_CURRENT_BINARY_DIR})
    set(current_dir ${CMAKE_CURRENT_SOURCE_DIR})
    set(stripped_dir)

    foreach(source ${ARGN})
        get_filename_component(source_path  "${source}"         ABSOLUTE)
        get_filename_component(source_dir   "${source_path}"    PATH)
        get_filename_component(source_name  "${source_path}"    NAME)

        if(source_dir MATCHES "^${builder_dir}.*$")
            set(prefix_dir "builder")
        elseif(source_dir MATCHES "^${binary_dir}.*$")
            set(prefix_dir "generated")
            string(REGEX REPLACE "^${binary_dir}/?(.*)$" "\\1" stripped_dir ${source_dir})
        elseif(source_dir MATCHES "^${current_dir}.*$")
            if(source_name MATCHES ".*([.]cmake).*")
                set(prefix_dir "builder")
            else()
                set(prefix_dir "sources")
            endif()
            string(REGEX REPLACE "^${current_dir}/?(.*)$" "\\1" stripped_dir ${source_dir})
        else()
            set(prefix_dir "${source_dir}")
        endif()

        if(stripped_dir)
            string(REPLACE "/" "\\\\" tmp ${stripped_dir})
            source_group("${prefix_dir}\\\\${tmp}" FILES ${source_path})
        else()
            source_group("${prefix_dir}" FILES ${source_path})
        endif()
    endforeach()
endfunction()

# --

macro(get_cxx_name name cxx_name)
    set(temp)
    string(TOLOWER "${name}" temp)
    string(REGEX REPLACE "(/)|(\\.)|(\\\\)|(::)|( )"  "_" temp "${temp}")
    string(REGEX REPLACE "^([0123456789])" "_\\1" temp "${temp}")
    string(REGEX REPLACE "([^ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_])" "_" temp "${temp}")
    set(${cxx_name} ${temp})
    unset(temp)
endmacro()

# --

macro(get_cpp_name name cpp_name)
    set(temp)
    string(REGEX REPLACE "([abcdefghijklmnopqrstuvwxyz])([ABCDEFGHIJKLMNOPQRSTUVWXYZ])" "\\1_\\2" temp "${name}")
    string(TOUPPER ${temp} temp)
    string(REGEX REPLACE "(/)|(\\.)|(\\\\)|(::)|( )"  "_" temp "${temp}")
    string(REGEX REPLACE "^([0123456789])" "_\\1" temp "${temp}")
    string(REGEX REPLACE "([^ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_])" "_" temp "${temp}")
    set(${cpp_name} ${temp})
    unset(temp)
endmacro()

# --

macro(_target_declare t)
    project(${t})

    set(${t}_KIND)
    set(${t}_PUBLIC_INCLUDE_DIRS)
    set(${t}_PRIVATE_INCLUDE_DIRS)
    set(${t}_INTERFACE_INCLUDE_DIRS)
    set(${t}_SOURCES)
    set(${t}_PUBLIC_DEFS)
    set(${t}_PRIVATE_DEFS)
    set(${t}_INTERFACE_DEFS)
    set(${t}_PUBLIC_COMPILE_FEATURES)
    set(${t}_PRIVATE_COMPILE_FEATURES)
    set(${t}_INTERFACE_COMPILE_FEATURES)
    set(${t}_PUBLIC_DEPS)
    set(${t}_PRIVATE_DEPS)
    set(${t}_INTERFACE_DEPS)
    set(${t}_INFO_PLIST)
    set(${t} OUTPUT_NAME)
endmacro()

# --

macro(_target_begin t)
    list(INSERT ${t}_PUBLIC_INCLUDE_DIRS 0
        ${${t}_SOURCE_DIR}
        ${${t}_BINARY_DIR}
    )
    list(INSERT ${t}_INTERFACE_INCLUDE_DIRS 0
        ${${t}_SOURCE_DIR}
        ${${t}_BINARY_DIR}
    )
endmacro()

# --

macro(_target_generate_api_h t)
    get_cpp_name(${t} CPP_NAME)
    get_filename_component(TEMPLATE_FILE "${BUILDER_SOURCE_DIR}/api.h.in" ABSOLUTE)
    configure_file("${BUILDER_SOURCE_DIR}/api.h.in" "${${t}_BINARY_DIR}/${t}_api.h" @ONLY)
    set_source_files_properties("${${t}_BINARY_DIR}/{${t}_api.h" PROPERTIES GENERATED TRUE)
    list(APPEND ${t}_SOURCES ${${t}_BINARY_DIR}/${t}_api.h)
    list(INSERT ${t}_PRIVATE_DEFS   0 "${CPP_NAME}_BUILD")
    list(INSERT ${t}_INTERFACE_DEFS 0 "${CPP_NAME}_LIB")
endmacro()

# --

macro(_target_generate_namespace_h t)
    get_cxx_name(${t} CXX_NAME)
    get_cpp_name(${t} CPP_NAME)
    get_filename_component(TEMPLATE_FILE "${BUILDER_SOURCE_DIR}/namespace.h.in" ABSOLUTE)
    configure_file("${BUILDER_SOURCE_DIR}/namespace.h.in" "${${t}_BINARY_DIR}/${t}_namespace.h" @ONLY)
    set_source_files_properties("${${t}_BINARY_DIR}/{${t}_namespace.h" PROPERTIES GENERATED TRUE)
    list(APPEND ${t}_SOURCES ${${t}_BINARY_DIR}/${t}_namespace.h)
endmacro()

# --

macro(_target_import_properties t)
    target_include_directories(${t} INTERFACE  ${${t}_INTERFACE_INCLUDE_DIRS})
    target_compile_definitions(${t} INTERFACE  ${${t}_INTERFACE_DEFS})
    target_compile_features(${t}    INTERFACE  ${${t}_INTERFACE_FEATURES})
    target_link_libraries(${t}      INTERFACE  ${${t}_INTERFACE_DEPS})
endmacro()

# --

macro(_target_populate_properties t)
    target_include_directories(${t} PUBLIC      ${${t}_PUBLIC_INCLUDE_DIRS})
    target_include_directories(${t} PRIVATE     ${${t}_PRIVATE_INCLUDE_DIRS})
    target_include_directories(${t} INTERFACE   ${${t}_INTERFACE_INCLUDE_DIRS})
    target_compile_definitions(${t} PUBLIC      ${${t}_PUBLIC_DEFS})
    target_compile_definitions(${t} PRIVATE     ${${t}_PRIVATE_DEFS})
    target_compile_definitions(${t} INTERFACE   ${${t}_INTERFACE_DEFS})
    target_compile_features(${t}    PUBLIC      ${${t}_PUBLIC_EATURES})
    target_compile_features(${t}    PRIVATE     ${${t}_PRIVATE_FEATURES})
    target_compile_features(${t}    INTERFACE   ${${t}_INTERFACE_FEATURES})
    target_link_libraries(${t}      PUBLIC      ${${t}_PUBLIC_DEPS})
    target_link_libraries(${t}      INTERFACE   ${${t}_INTERFACE_DEPS})
    target_link_libraries(${t}      PRIVATE     ${${t}_PRIVATE_DEPS})

    get_target_property(sources ${t} SOURCES)
    group_sources(${sources})

    set_target_properties(${t} PROPERTIES
                          PROJECT_LABEL     "${${t}_KIND} ${t}"
                          OUTPUT_NAME       "${${t}_OUTPUT_NAME}"
    )
endmacro()

# --

macro(_target_end t)

endmacro()

# --

macro(target t)
    _target_declare(${t})
endmacro()

# --

macro(target_include_dirs)
    list(APPEND ${PROJECT_NAME}_PUBLIC_INCLUDE_DIRS ${ARGN})
endmacro()

# --

macro(target_private_include_dirs)
    list(APPEND ${PROJECT_NAME}_PRIVATE_INCLUDE_DIRS ${ARGN})
endmacro()

# --

macro(target_interface_include_dirs)
    list(APPEND ${PROJECT_NAME}_INTERFACE_INCLUDE_DIRS ${ARGN})
endmacro()

# --

macro(target_sources)
    list(APPEND ${PROJECT_NAME}_SOURCES ${ARGN})
endmacro()

# --

macro(target_defs)
    list(APPEND ${PROJECT_NAME}_PUBLIC_DEFS ${ARGN})
endmacro()

# --

macro(target_private_defs)
    list(APPEND ${PROJECT_NAME}_PRIVATE_DEFS ${ARGN})
endmacro()

# --

macro(target_interface_defs)
    list(APPEND ${PROJECT_NAME}_INTERFACE_DEFS ${ARGN})
endmacro()

# --

macro(target_deps)
    list(APPEND ${PROJECT_NAME}_PUBLIC_DEPS ${ARGN})
endmacro()

# --

macro(target_private_deps)
    list(APPEND ${PROJECT_NAME}_PRIVATE_DEPS ${ARGN})
endmacro()

# --

macro(target_interface_deps)
    list(APPEND ${PROJECT_NAME}_INTERFACE_DEPS ${ARGN})
endmacro()

# --

macro(target_features)
    list(APPEND ${PROJECT_NAME}_PUBLIC_FEATURES ${ARGN})
endmacro()

# --

macro(target_private_features)
    list(APPEND ${PROJECT_NAME}_PRIVATE_FEATURES ${ARGN})
endmacro()

# --

macro(target_interface_features)
    list(APPEND ${PROJECT_NAME}_INTERFACE_FEATURES ${ARGN})
endmacro()

# --

macro(target_info_plist info_plist)
    set(${PROJECT_NAME}_INFO_PLIST ${info_plist})
endmacro()

# --

macro(target_output_name name)
    set(${PROJECT_NAME}_OUTPUT_NAME ${name})
endmacro()

# --

macro(import_static_library)
    _target_begin(${PROJECT_NAME})
    add_library(${PROJECT_NAME} INTERFACE)
    _target_import_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()

# --

macro(import_shared_library)
    _target_begin(${PROJECT_NAME})
    add_library(${PROJECT_NAME} INTERFACE)
    _target_import_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()

# --

macro(import_headers_library)
    _target_begin(${PROJECT_NAME})
    add_library(${PROJECT_NAME} INTERFACE)
    _target_import_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()

# --

macro(add_external_headers_library)
    _target_begin(${PROJECT_NAME})
    add_library(${PROJECT_NAME} INTERFACE)
    _target_import_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()

# --

macro(add_external_static_library)
    _target_begin(${PROJECT_NAME})
    set(${PROJECT_NAME}_KIND "[STATIC LIB]")
    add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
    _target_populate_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()

# --

macro(add_external_shared_library)
    _target_begin(${PROJECT_NAME})
    set(${PROJECT_NAME}_KIND "[SHARED LIB]")
    add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})
    _target_populate_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()

# --

macro(add_headers_library)
    _target_begin(${PROJECT_NAME})
    _target_generate_api_h(${PROJECT_NAME})
    _target_generate_namespace_h(${PROJECT_NAME})
    add_library(${PROJECT_NAME} INTERFACE)
    _target_import_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()

# --

macro(add_static_library)
    _target_begin(${PROJECT_NAME})
    _target_generate_api_h(${PROJECT_NAME})
    _target_generate_namespace_h(${PROJECT_NAME})
    set(${PROJECT_NAME}_KIND "[STATIC LIB]")
    add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES})
    _target_populate_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()

# --

macro(add_shared_library)
    _target_begin(${PROJECT_NAME})
    _target_generate_api_h(${PROJECT_NAME})
    _target_generate_namespace_h(${PROJECT_NAME})
    set(${PROJECT_NAME}_KIND "[SHARED LIB]")
    add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SOURCES})
    _target_populate_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()

# --

macro(add_console_application)
    _target_begin(${PROJECT_NAME})
    _target_generate_api_h(${PROJECT_NAME})
    _target_generate_namespace_h(${PROJECT_NAME})
    set(${PROJECT_NAME}_KIND "[CONSOLE APP]")
    add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
    _target_populate_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()

# --

macro(add_application)
    _target_begin(${PROJECT_NAME})
    _target_generate_api_h(${PROJECT_NAME})
    _target_generate_namespace_h(${PROJECT_NAME})
    set(${PROJECT_NAME}_KIND "[APP]")
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32 ${${PROJECT_NAME}_SOURCES})
    if(MACOS_BUILD)
        if(${PROJECT_NAME}_INFO_PLIST)
            set_target_properties(${PROJECT_NAME} PROPERTIES
                                  MACOSX_BUNDLE_INFO_PLIST "${${PROJECT_NAME}_SOURCE_DIR}/${${PROJECT_NAME}_INFO_PLIST}"
            )
        endif()
    endif()
    _target_populate_properties(${PROJECT_NAME})
    _target_end(${PROJECT_NAME})
endmacro()
