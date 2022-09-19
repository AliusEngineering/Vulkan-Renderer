#pragma once

#include "VulkanRendererPch.hpp"

#include "Pipeline/Components/Instance.hpp"

namespace AliusModules {

struct SwapchainSupportDetails
{
  vk::SurfaceCapabilitiesKHR SurfaceCaps;
  std::vector<vk::SurfaceFormatKHR> SurfaceFormats;
  std::vector<vk::PresentModeKHR> PresentModes;
};

struct SwapchainMeta
{
  vk::Format ImageFormat;
  vk::Extent2D Extent;
};

class Swapchain
{
public:
  Swapchain(const Instance& instance);

  vk::SwapchainKHR GetSwapchain() const { return m_Swapchain; }

  vk::Queue GetPresentQueue() const { return m_PresentQueue; }
  vk::Queue GetComputeQueue() const { return m_ComputeQueue; }

  SwapchainMeta GetMeta() const
  {
	return { m_SwapchainImageFormat, m_SwapchainExtent };
  }

  uint32_t GetMaxConcurrentFrames() const { return m_MaxConcurrentFrames; }

  std::vector<vk::ImageView> GetImageViews() const { return m_ImageViews; }

  bool Recreate();

  void Cleanup();

private:
  Instance m_Instance;

  vk::SwapchainKHR m_Swapchain;

  vk::Format m_SwapchainImageFormat;
  vk::Extent2D m_SwapchainExtent;

  vk::Queue m_ComputeQueue, m_PresentQueue;

  std::vector<vk::Image> m_Images{};
  std::vector<vk::ImageView> m_ImageViews{};

private:
  vk::SwapchainKHR CreateSwapchain();

  std::vector<vk::Image> CreateImages(const vk::Device& device,
                                      const vk::SwapchainKHR& swapchain);
  std::vector<vk::ImageView> CreateImageViews(
    const vk::Device& device,
    const vk::SwapchainKHR& swapchain,
    const std::vector<vk::Image>& images);

private:
  uint32_t m_MaxConcurrentFrames = 0;
};

} // AliusModules
