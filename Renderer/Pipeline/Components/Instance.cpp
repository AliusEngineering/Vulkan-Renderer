#include "Instance.hpp"

#include "Window/VulkanWindow.hpp"

namespace AliusModules {

struct VkfwRequiredExtensions
{
  VkfwRequiredExtensions()
  {
	Extensions = VulkanWindow::GetRequiredInstanceExtensions(&Count);
  }

  uint32_t Count = 0;
  const char** Extensions;
};

Instance::Instance()
{
  m_Instance = CreateInstance();
  m_PhysicalDevice = PickPhysicalDevice(m_Instance);
  m_Surface = VulkanWindow::CreateSurfaceKHR(m_Instance);
  m_Device = CreateDevice(m_PhysicalDevice);
}

vk::Instance
Instance::CreateInstance()
{
  VkfwRequiredExtensions requiredInstanceExtensions{};

  if (g_ValidationLayers && !InstanceHelpers::CheckValidationLayersSupport()) {
	SQD_ERR("Validation layers requested but not available!");
	throw std::runtime_error("Validation layers requested but not available!");
  }

  c_InstanceInfo.enabledExtensionCount = requiredInstanceExtensions.Count;
  c_InstanceInfo.ppEnabledExtensionNames =
    requiredInstanceExtensions.Extensions;

  if (g_ValidationLayers) {
	c_InstanceInfo.enabledLayerCount = g_RequiredLayers.size();
	c_InstanceInfo.ppEnabledLayerNames = g_RequiredLayers.data();
  } else {
	c_InstanceInfo.enabledLayerCount = 0;
  }

  c_AppInfo.setApiVersion(InstanceHelpers::GetAvailableVulkanVersion());

  SQD_LOG("Creating Vulkan instance with API version: {}.{}",
          VK_API_VERSION_MAJOR(c_AppInfo.apiVersion),
          VK_API_VERSION_MINOR(c_AppInfo.apiVersion));

  TRY_EXCEPT(return vk::createInstance(c_InstanceInfo))
  THROW_ANY("Failed to create Vulkan instance!")
}

vk::PhysicalDevice
Instance::PickPhysicalDevice(const vk::Instance& instance)
{
  auto devices = instance.enumeratePhysicalDevices();
  for (const auto& device : devices) {
	if (InstanceHelpers::IsDeviceSuitable(device))
	  return device;
  }

  SQD_ERR("Failed to pick physical Vulkan device");
  throw std::runtime_error("Failed to pick physical Vulkan device");
}

vk::Device
Instance::CreateDevice(const vk::PhysicalDevice& physicalDevice)
{
  m_QueueFamilies =
    InstanceHelpers::ResolveQueueFamilies(physicalDevice, m_Surface);

  std::set<uint32_t> createQueues = { m_QueueFamilies.Compute,
	                                  m_QueueFamilies.Present };

  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{};
  queueCreateInfos.reserve(createQueues.size());
  for (const auto& queue : createQueues) {
	queueCreateInfos.emplace_back(
	  vk::DeviceQueueCreateFlags{}, queue, 1, &c_BaseQueuePriority);
  }

  vk::DeviceCreateInfo createInfo(
    {},
    queueCreateInfos.size(),
    queueCreateInfos.data(),
    g_ValidationLayers ? g_RequiredLayers.size() : 0,
    g_ValidationLayers ? g_RequiredLayers.data() : nullptr,
    g_RequiredExtensions.size(),
    g_RequiredExtensions.data());
  createInfo.pEnabledFeatures = &g_RequiredPhysicalDeviceFeatures;

  TRY_EXCEPT(return physicalDevice.createDevice(createInfo))
  THROW_ANY("Failed to create logical Vulkan device!")
}

void
Instance::Cleanup()
{
  m_Instance.destroySurfaceKHR(m_Surface);
  m_Device.destroy();
}

} // AliusModules