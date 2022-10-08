#pragma once

#include "VulkanRendererPch.hpp"

namespace AliusModules {

struct QueueFamilies
{
  QueueFamilies() = default;

  uint32_t Compute;
  uint32_t Present;
};

class Instance
{
public:
  explicit Instance();

  vk::Instance& GetVkInstance() { return m_Instance; }
  vk::PhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }
  vk::SurfaceKHR& GetSurface() { return m_Surface; }
  vk::Device& GetDevice() { return m_Device; }

  QueueFamilies& GetQueueFamilies() { return m_QueueFamilies; }

  void Cleanup();

private:
  vk::Instance CreateInstance();
  vk::PhysicalDevice PickPhysicalDevice(const vk::Instance& instance);
  vk::Device CreateDevice(const vk::PhysicalDevice& physicalDevice);

private:
  vk::Instance m_Instance;
  vk::PhysicalDevice m_PhysicalDevice;
  vk::SurfaceKHR m_Surface;
  vk::Device m_Device;

  QueueFamilies m_QueueFamilies{};

private:
  vk::ApplicationInfo c_AppInfo = { nullptr,
	                                c_AppVersion,
	                                nullptr,
	                                c_AppVersion };

  vk::InstanceCreateInfo c_InstanceInfo = { {}, &c_AppInfo };

  inline static constexpr uint32_t c_AppVersion =
    VK_MAKE_API_VERSION(0, 1, 0, 0);

  inline static constexpr float c_BaseQueuePriority = 1.0f;
};

} // AliusModules
