#pragma once

#include "VulkanRendererPch.hpp"

#include "Pipeline/Components/Instance.hpp"

namespace AliusModules {

struct SwapchainSupportDetails;

class Swapchain
{
public:
  explicit Swapchain(Ref<Instance> instance);

  vk::SwapchainKHR& GetSwapchain() { return m_Swapchain; }
  const vk::SwapchainKHR& GetSwapchain() const { return m_Swapchain; }

  vk::Queue& GetPresentQueue() { return m_PresentQueue; }
  const vk::Queue& GetPresentQueue() const { return m_PresentQueue; }
  vk::Queue& GetComputeQueue() { return m_ComputeQueue; }
  const vk::Queue& GetComputeQueue() const { return m_ComputeQueue; }

  uint32_t GetMaxConcurrentFrames() const { return m_MaxConcurrentFrames; }

  std::vector<vk::ImageView>& GetImageViews() { return m_ImageViews; }
  const std::vector<vk::ImageView>& GetImageViews() const
  {
	return m_ImageViews;
  }

  void CleanupBeforeRecreate();
  bool Recreate();

  void Cleanup();

  vk::Extent2D Extent{};
  vk::Format ImageFormat = vk::Format::eUndefined;

private:
  Ref<Instance> m_Instance;

  vk::SwapchainKHR m_Swapchain, m_OldSwapchain;

  vk::Queue m_ComputeQueue, m_PresentQueue;

  std::vector<vk::Image> m_Images{};
  std::vector<vk::ImageView> m_ImageViews{};

private:
  vk::SwapchainKHR CreateSwapchain(
    const vk::SwapchainKHR& oldSwapchain = nullptr);

  std::vector<vk::Image> CreateImages(const vk::Device& device,
                                      const vk::SwapchainKHR& swapchain);

  std::vector<vk::ImageView> CreateImageViews(
    const vk::Device& device,
    const vk::SwapchainKHR& swapchain,
    const std::vector<vk::Image>& images);

private:
  uint32_t m_MaxConcurrentFrames = 0;
};

struct SwapchainSupportDetails
{
  vk::SurfaceCapabilitiesKHR SurfaceCaps;
  std::vector<vk::SurfaceFormatKHR> SurfaceFormats;
  std::vector<vk::PresentModeKHR> PresentModes;
};

} // AliusModules
