#pragma once

#include "vulkan/vulkan.hpp"

#include "Logger.hpp"

#include "Pipeline/VulkanHelpers.hpp"

#include "Globals.hpp"

#include "set"
#include "unordered_map"
#include "vector"

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
