list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

project(builder)

if(NOT BUILDER_ROOT_PROJECT_NAME)
    get_filename_component(BUILDER_ROOT_PROJECT_NAME "${CMAKE_SOURCE_DIR}" NAME CACHE)
endif()

set(BUILDER_SOURCE_DIR ${builder_SOURCE_DIR} CACHE PATH "Builder's directory.")
set(BUILDER_BINARY_DIR ${builder_BINARY_DIR} CACHE PATH "Builder's binary directory.")

get_filename_component(build_root_source_dir "${BUILDER_SOURCE_DIR}/.." ABSOLUTE)
get_filename_component(build_root_binary_dir "${BUILDER_BINARY_DIR}/.." ABSOLUTE)

set(BUILD_ROOT_SOURCE_DIR ${build_root_source_dir} CACHE PATH "Build root source directory.")
set(BUILD_ROOT_BINARY_DIR ${build_root_binary_dir} CACHE PATH "Build root binary directory.")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${BUILD_ROOT_BINARY_DIR} CACHE PATH "Archive output directory.")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${BUILD_ROOT_BINARY_DIR} CACHE PATH "Library output directory.")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${BUILD_ROOT_BINARY_DIR} CACHE PATH "Runtime output directory.")

mark_as_advanced(
    BUILDER_SOURCE_DIR
    BUILDER_BINARY_DIR
    BUILD_ROOT_SOURCE_DIR
    BUILD_ROOT_BINARY_DIR
    CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    CMAKE_LIBRARY_OUTPUT_DIRECTORY
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(platform)
include(target)
include(qt)
