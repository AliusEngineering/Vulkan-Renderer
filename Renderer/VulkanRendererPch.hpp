#pragma once

#include "vulkan/vulkan.hpp"

#include "Logger.hpp"

#include "Pipeline/VulkanHelpers.hpp"

#include "set"
#include "unordered_map"
#include "vector"

std::vector<const char*> g_RequiredExtensions = {};

std::vector<const char*> g_RequiredLayers = { "VK_LAYER_KHRONOS_validation" };

vk::PhysicalDeviceFeatures g_RequiredPhysicalDeviceFeatures{};

#ifdef ALS_DEBUG
constexpr bool g_ValidationLayers = true;
#else
constexpr bool g_ValidationLayers = false;
#endif

// Exception handling
#define TRY_EXCEPT(...)                                                        \
  try {                                                                        \
	__VA_ARGS__;                                                               \
  }

#define THROW_ANY(message)                                                     \
  catch (...)                                                                  \
  {                                                                            \
	SQD_ERR(message);                                                          \
	throw std::runtime_error(message);                                         \
  }
#define WARN_ANY(message)                                                      \
  catch (...)                                                                  \
  {                                                                            \
	SQD_WARN(message);                                                         \
  }
