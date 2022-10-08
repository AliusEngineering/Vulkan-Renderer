cmake_minimum_required(VERSION 3.16)
project(VulkanRendererSandbox)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (NOT TARGET VulkanRendererSandbox)
    add_executable(VulkanRendererSandbox main.cpp)
endif ()

target_link_libraries(VulkanRendererSandbox PUBLIC AlsVkRenderer)

add_custom_command(TARGET VulkanRendererSandbox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${AlsVkRenderer_SOURCE_DIR}/Resources" "${PROJECT_BINARY_DIR}/Resources"
        )
