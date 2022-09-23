#include "Globals.hpp"

std::vector<const char*> g_RequiredExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::vector<const char*> g_RequiredLayers = {
  "VK_LAYER_KHRONOS_validation"
};

vk::PhysicalDeviceFeatures g_RequiredPhysicalDeviceFeatures{};