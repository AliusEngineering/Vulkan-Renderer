#include "VulkanRendererPch.hpp"

#include "VulkanHelpers.hpp"

#include "Pipeline/Components/Instance.hpp"
#include "Pipeline/Components/Swapchain.hpp"

#include "Window/VulkanWindow.hpp"

#include "algorithm"

namespace AliusModules {

bool
InstanceHelpers::CheckValidationLayersSupport()
{
  std::vector<vk::LayerProperties> layers =
    vk::enumerateInstanceLayerProperties();

  for (auto layerName : g_RequiredLayers) {
	bool layerFound = false;

	for (const auto& layerProperties : layers) {
	  if (strcmp(layerName, layerProperties.layerName) == 0) {
		layerFound = true;
		break;
	  }
	}

	if (!layerFound) {
	  return false;
	}
  }

  return true;
}

bool
InstanceHelpers::IsDeviceSuitable(const vk::PhysicalDevice& device)
{
  if (!device.getFeatures().geometryShader)
	return false;

  if (!InstanceHelpers::CheckExtensionSupport(device, g_RequiredExtensions))
	return false;

  return true;
}

bool
InstanceHelpers::CheckExtensionSupport(
  const vk::PhysicalDevice& device,
  const ::std::vector<const char*>& extensionNames)
{

  auto extensions = device.enumerateDeviceExtensionProperties(nullptr);
  std::set<std::string> extensionSet(extensionNames.begin(),
                                     extensionNames.end());

  for (const auto& extension : extensions) {
	extensionSet.erase(extension.extensionName);
  }

  return extensionSet.empty();
}

uint32_t
InstanceHelpers::GetAvailableVulkanVersion()
{
  auto ver = vk::enumerateInstanceVersion();
  if (VK_API_VERSION_MAJOR(ver) >= 1) {
	switch (VK_API_VERSION_MINOR(ver)) {
	  case 0:
		return VK_API_VERSION_1_0;
	  case 1:
		return VK_API_VERSION_1_1;
	  case 2:
		return VK_API_VERSION_1_2;
	  case 3:
		return VK_API_VERSION_1_3;
	  default:
		SQD_ERR("Unsupported minor Vulkan API version!");
		throw std::runtime_error("Unsupported minor Vulkan API version!");
	}
  } else {
	SQD_ERR("Unsupported major Vulkan API version!");
	throw std::runtime_error("Unsupported major Vulkan API version!");
  }
}

QueueFamilies
InstanceHelpers::ResolveQueueFamilies(const vk::PhysicalDevice& device,
                                      vk::SurfaceKHR& targetSurface)
{
  QueueFamilies ret{};
  auto queueFamilies = device.getQueueFamilyProperties();

  uint32_t index = 0;
  for (const auto& queueFamily : queueFamilies) {
	if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics &&
	    queueFamily.queueFlags & vk::QueueFlagBits::eCompute &&
	    device.getSurfaceSupportKHR(index, targetSurface)) {
	  return { index, index };
	}

	index++;
  }

  index = 0;
  for (const auto& queueFamily : queueFamilies) {
	if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) {
	  ret.Compute = index;
	  break;
	}

	index++;
  }

  index = 0;
  for (const auto& queueFamily : queueFamilies) {
	if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics &&
	    device.getSurfaceSupportKHR(index, targetSurface)) {
	  ret.Present = index;
	  break;
	}

	index++;
  }

  return ret;
}

SwapchainSupportDetails
SwapchainHelpers::QuerySupportDetails(const vk::PhysicalDevice& physicalDevice,
                                      const vk::SurfaceKHR& surface)
{
  auto surfaceCaps = physicalDevice.getSurfaceCapabilitiesKHR(surface);
  auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
  auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

  return { surfaceCaps, surfaceFormats, presentModes };
}

vk::SurfaceFormatKHR
SwapchainHelpers::PickSurfaceFormat(
  const std::vector<vk::SurfaceFormatKHR>& formats)
{
  if (formats.empty())
	return {};

  for (const auto& format : formats) {
	if (format == c_PreferredSurfaceFormat)
	  return format;
  }

  for (const auto& format : formats) {
	if (format.format == c_PreferredVkFormat)
	  return format;
  }

  for (const auto& format : formats) {
	if (format.colorSpace == c_PreferredVkColorspace)
	  return format;
  }

  return formats.at(0);
}

vk::PresentModeKHR
SwapchainHelpers::PickPresentMode(
  const std::vector<vk::PresentModeKHR>& presentModes)
{
  for (const auto& mode : presentModes) {
	if (mode == c_PreferredPresentMode)
	  return mode;
  }

  return vk::PresentModeKHR::eFifo;
}

vk::Extent2D
SwapchainHelpers::PickExtend2D(
  const vk::SurfaceCapabilitiesKHR& surfaceCapabilities)
{
  if (surfaceCapabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
	return surfaceCapabilities.currentExtent;
  } else {
	auto [width, height] = VulkanWindow::GetWindow().getFramebufferSize();

	VkExtent2D extent = { static_cast<uint32_t>(width),
	                      static_cast<uint32_t>(height) };

	extent.width = std::clamp(extent.width,
	                          surfaceCapabilities.minImageExtent.width,
	                          surfaceCapabilities.maxImageExtent.width);
	extent.height = std::clamp(extent.height,
	                           surfaceCapabilities.minImageExtent.height,
	                           surfaceCapabilities.maxImageExtent.height);

	return extent;
  }
}

uint32_t
SwapchainHelpers::PickOptimalConcurrentFrames(
  const vk::PresentModeKHR& presentMode)
{
  if (presentMode == vk::PresentModeKHR::eMailbox)
	return 3;
  else
	return 2;
}

vk::Queue
SwapchainHelpers::GetQueue(const vk::Device& device,
                           uint32_t familyIndex,
                           uint32_t queueIndex)
{
  return device.getQueue(familyIndex, queueIndex);
}

} // AliusModules