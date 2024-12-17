# CPMAddPackage(
# NAME boost
# GITHUB_REPOSITORY boostorg/boost
# VERSION 1.70.0
# GIT_TAG boost-1.70.0
# )

set(SDL_STATIC ON)

CPMAddPackage(
    NAME SDL2
    GITHUB_REPOSITORY libsdl-org/SDL
    GIT_TAG release-2.28.3
    VERSION 2.28.3
)
find_package(SDL2 REQUIRED)

CPMAddPackage("gh:g-truc/glm#master") # GLM

find_package(Vulkan REQUIRED)
set(DEPENDENCIES
    SDL2::SDL2
    Vulkan::Vulkan
    glm::glm
)