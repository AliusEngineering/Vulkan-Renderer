#pragma once

#include "vulkan/vulkan.hpp"

#include "vector"

extern std::vector<const char*> g_RequiredExtensions;
extern std::vector<const char*> g_RequiredLayers;
extern vk::PhysicalDeviceFeatures g_RequiredPhysicalDeviceFeatures;
