#pragma once

#include "Globals.hpp"

#include "Logger.hpp"

#include "vulkan/vulkan.hpp"

#include "Pipeline/VulkanHelpers.hpp"

/* Renderer objects */
#include "RendererBase/Objects/RendererObjects.hpp"

#include "Objects/VkObjectData.hpp"

/* STL */
#include "set"
#include "unordered_map"
#include "vector"

template<class C>
using Ref = std::shared_ptr<C>;

#ifdef ALS_DEBUG
constexpr bool g_ValidationLayers = true;
#elif ALS_RELEASE
constexpr bool g_ValidationLayers = false;
#define SQD_DISABLE_LOGGER 1
#else
/* We still might want logging on, but without Vulkan validation layers
 * performance overhead.
 */
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
