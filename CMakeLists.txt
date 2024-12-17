cmake_minimum_required(VERSION 3.28.3)
project(
    Vanadium
    VERSION 4.0.0
    DESCRIPTION "Vanadium Vulkan Renderer"
    LANGUAGES CXX C ${EXTRA_LANGUAGES}
)

# set cpp 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_BINARY_DIR})

# # file globbing
file(GLOB_RECURSE sources src/main/*.cpp src/main/*.h)

add_executable(vanadium ${sources})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT vanadium)

# # Target definitions
add_subdirectory(builder)
builder_configure_platform()

# include files relative to root of src
target_include_directories(vanadium PUBLIC src/main)

# # Dependencies

# add cmake package manager
include(cmake/CPM.cmake)

include(Dependencies.cmake)

target_link_libraries(vanadium PUBLIC ${DEPENDENCIES})

# # Packaging
install(TARGETS vanadium DESTINATION vanadium_destination)
install(DIRECTORY resources DESTINATION vanadium_destination)
