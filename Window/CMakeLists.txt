cmake_minimum_required(VERSION 3.16)
project(AlsVkWindow)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("3rd-party/glfw")

file(GLOB_RECURSE AlsVkWindowSources CONFIGURE_DEPENDS "Window/**")
message(STATUS "[AlsVkWindow] Source are: ${AlsVkWindowSources}")

add_library(AlsVkWindow ${AlsVkWindowSources})

target_link_libraries(AlsVkWindow PUBLIC ${LOGGER_LIBS} glfw)

target_include_directories(AlsVkWindow PUBLIC
        ${LOGGER_INCLUDES}
        ../dependencies/RendererBase/WindowBase
        3rd-party/vkfw/include
        )