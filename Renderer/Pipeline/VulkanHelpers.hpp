#pragma once

#include "cstdint"
#include "vector"

namespace vk {
class PhysicalDevice;
class SurfaceKHR;

class SurfaceFormatKHR;

enum class Format;
enum class PresentModeKHR;
enum class ColorSpaceKHR;

class Extent2D;
}

namespace AliusModules {

struct QueueFamilies;
struct SwapchainSupportDetails;

class InstanceHelpers
{
public:
  static bool CheckValidationLayersSupport();
  static bool IsDeviceSuitable(const vk::PhysicalDevice& device);
  static bool CheckExtensionSupport(
    const vk::PhysicalDevice& device,
    const std::vector<const char*>& extensionNames);
  static uint32_t GetAvailableVulkanVersion();
  static QueueFamilies ResolveQueueFamilies(const vk::PhysicalDevice& device,
                                            vk::SurfaceKHR& targetSurface);
};

class SwapchainHelpers
{
public:
  static SwapchainSupportDetails QuerySupportDetails(
    const vk::PhysicalDevice& physicalDevice,
    const vk::SurfaceKHR& surface);

  static vk::SurfaceFormatKHR PickSurfaceFormat(
    const std::vector<vk::SurfaceFormatKHR>& formats);
  static vk::PresentModeKHR PickPresentMode(
    const std::vector<vk::PresentModeKHR>& presentModes);
  static vk::Extent2D PickExtend2D(
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities);
  static uint32_t PickOptimalConcurrentFrames(
    const vk::PresentModeKHR& presentMode);
  static vk::Queue GetQueue(const vk::Device& device,
                            uint32_t familyIndex,
                            uint32_t queueIndex = 0);

private:
  inline static constexpr vk::PresentModeKHR c_PreferredPresentMode =
    vk::PresentModeKHR::eMailbox;

  inline static constexpr vk::Format c_PreferredVkFormat =
    vk::Format::eB8G8R8A8Srgb;
  inline static constexpr vk::ColorSpaceKHR c_PreferredVkColorspace =
    vk::ColorSpaceKHR::eSrgbNonlinear;

  inline static constexpr vk::SurfaceFormatKHR c_PreferredSurfaceFormat =
    vk::SurfaceFormatKHR(c_PreferredVkFormat, c_PreferredVkColorspace);
};

class CommandPipelineHelpers
{
public:
  static uint32_t FindMemoryType(const vk::PhysicalDevice& physicalDevice,
                                 uint32_t typeFilter,
                                 vk::MemoryPropertyFlags memoryProperties);
};

} // AliusModules
