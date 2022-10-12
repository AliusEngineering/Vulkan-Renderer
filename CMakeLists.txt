# Latest feature: precompiled headers (3.16+)
cmake_minimum_required(VERSION 3.16)
project(AlsVkRenderer)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

link_libraries(Vulkan::Vulkan)

# Compile time definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "[AlsVkRenderer] Building in Debug.")
    add_compile_definitions(ALS_DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "[AlsVkRenderer] Building in Release.")
    add_compile_definitions(ALS_RELEASE SQD_DISABLE_LOGGER=1)
endif ()

if (NOT DEFINED ALS_BINARY_LOCATION)
    message(STATUS "[AlsVkRenderer] ALS_BINARY_LOCATION not set! Setting it to be current project binary output directory")
    set(ALS_BINARY_LOCATION ${PROJECT_BINARY_DIR})
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

add_library(AlsVkRenderer OBJECT ${AlsVkRendererSources})

target_precompile_headers(AlsVkRenderer PUBLIC "Renderer/VulkanRendererPch.hpp")

# VulkanRenderer Base includes
add_subdirectory("dependencies/RendererBase")
target_link_libraries(AlsVkRenderer PUBLIC RendererBase)
target_include_directories(AlsVkRenderer PUBLIC "dependencies/RendererBase")

target_include_directories(AlsVkRenderer PUBLIC
        Renderer
        Window
        ${LOGGER_INCLUDES}
        )

target_link_libraries(AlsVkRenderer PUBLIC
        ${LOGGER_LIBS}
        AlsVkWindow
        shaderc
        )

# Include sandbox application
if (NOT DEFINED ${ALS_VK_RENDERER_BUILD_SANDBOX})
    if (${ALS_BUILD_SANDBOXES} ON)
        set(ALS_VK_RENDERER_BUILD_SANDBOX ON)
    else ()
        set(ALS_VK_RENDERER_BUILD_SANDBOX OFF)
    endif ()
endif ()

message(STATUS "[AlsVkRenderer] Building Sandbox: ${ALS_VK_RENDERER_BUILD_SANDBOX}")

if (${ALS_VK_RENDERER_BUILD_SANDBOX})
    add_subdirectory("Sandbox")
endif ()