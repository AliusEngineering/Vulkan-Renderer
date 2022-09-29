# Latest feature: precompiled headers (3.16+)
cmake_minimum_required(VERSION 3.16)
project(AlsVkRenderer)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

# Compile time definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "[AlsVkRenderer] Building in Debug.")
    add_compile_definitions(ALS_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "[AlsVkRenderer] Building in Release.")
    add_compile_definitions(ALS_RELEASE)
endif ()

if (NOT LOGGER_LIBS)
    message(STATUS "Untitled-logger not found! Including it into build process")
    add_subdirectory("3rd-party/logger")
endif ()

set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
add_subdirectory("3rd-party/shaderc")

add_subdirectory("Window")

# Compile target - glob everything under ./Renderer
file(GLOB_RECURSE AlsVkRendererSources CONFIGURE_DEPENDS "Renderer/**")
message(STATUS "[AlsVkRenderer] Sources are: ${AlsVkRendererSources}")

add_library(AlsVkRenderer ${AlsVkRendererSources})

target_precompile_headers(AlsVkRenderer PUBLIC "Renderer/VulkanRendererPch.hpp")

# VulkanRenderer Base includes
add_subdirectory("dependencies/RendererBase")
target_link_libraries(AlsVkRenderer PUBLIC RendererBase)
target_include_directories(AlsVkRenderer PUBLIC "dependencies/RendererBase")

target_include_directories(AlsVkRenderer PUBLIC
        Renderer
        Window
        ${Vulkan_INCLUDE_DIRS}
        ${LOGGER_INCLUDES}
        )

target_link_libraries(AlsVkRenderer PUBLIC
        ${Vulkan_LIBRARIES}
        ${LOGGER_LIBS}
        AlsVkWindow
        shaderc
        )

# Custom variables

## Use this to copy your external shaders over and use them in Renderer
set(ALS_VK_RENDERER_SHADER_PATH "Resources/Shaders/WorldObject")

# Include sandbox application
if (NOT DEFINED ${ALS_VK_RENDERER_BUILD_SANDBOX})
    if (${ALS_BUILD_SANDBOXES} ON)
        set(ALS_VK_RENDERER_BUILD_SANDBOX ON)
    else ()
        set(ALS_VK_RENDERER_BUILD_SANDBOX OFF)
    endif ()
endif ()

message(STATUS "[AlsVkRenderer] Building Sandbox: ${ALS_VK_RENDERER_BUILD_SANDBOX}")

add_subdirectory("Sandbox")