#include "Swapchain.hpp"

namespace AliusModules {

Swapchain::Swapchain(const Instance& instance)
  : m_Instance(instance)
{
  m_Swapchain = CreateSwapchain();
  m_ComputeQueue = SwapchainHelpers::GetQueue(
    m_Instance.GetDevice(), m_Instance.GetQueueFamilies().Compute);
  m_PresentQueue = SwapchainHelpers::GetQueue(
    m_Instance.GetDevice(), m_Instance.GetQueueFamilies().Present);

  m_Images = CreateImages(m_Instance.GetDevice(), m_Swapchain);
  m_ImageViews =
    CreateImageViews(m_Instance.GetDevice(), m_Swapchain, m_Images);
}

vk::SwapchainKHR
Swapchain::CreateSwapchain()
{
  auto supportDetails = SwapchainHelpers::QuerySupportDetails(
    m_Instance.GetPhysicalDevice(), m_Instance.GetSurface());

  if (supportDetails.PresentModes.empty() ||
      supportDetails.SurfaceFormats.empty()) {
	SQD_ERR("Selected physical devices does not support swapchain!");
	throw std::runtime_error(
	  "Selected physical devices does not support swapchain!");
  }

  auto format =
    SwapchainHelpers::PickSurfaceFormat(supportDetails.SurfaceFormats);
  auto presentMode =
    SwapchainHelpers::PickPresentMode(supportDetails.PresentModes);
  m_MaxConcurrentFrames =
    SwapchainHelpers::PickOptimalConcurrentFrames(presentMode);
  auto extent = SwapchainHelpers::PickExtend2D(supportDetails.SurfaceCaps);

  m_SwapchainImageFormat = format.format;
  m_SwapchainExtent = extent;

  uint32_t imageCount = std::clamp(supportDetails.SurfaceCaps.minImageCount + 3,
                                   supportDetails.SurfaceCaps.minImageCount,
                                   supportDetails.SurfaceCaps.maxImageCount != 0
                                     ? supportDetails.SurfaceCaps.maxImageCount
                                     : 100);

  vk::SwapchainCreateInfoKHR createInfo{
	{},
	m_Instance.GetSurface(),
	imageCount,
	format.format,
	format.colorSpace,
	extent,
	1,
	vk::ImageUsageFlagBits::eColorAttachment
  };

  auto queueFamilies = m_Instance.GetQueueFamilies();

  if (queueFamilies.Present != queueFamilies.Compute) {
	createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
	createInfo.queueFamilyIndexCount = 2;
	createInfo.pQueueFamilyIndices =
	  std::array<uint32_t, 2>({ queueFamilies.Present, queueFamilies.Compute })
	    .data();
  } else {
	createInfo.imageSharingMode = vk::SharingMode::eExclusive;
  }

  createInfo.preTransform = supportDetails.SurfaceCaps.currentTransform;
  createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = nullptr;

  TRY_EXCEPT(return m_Instance.GetDevice().createSwapchainKHR(createInfo))
  THROW_ANY("Failed to create swapchain!")
}

bool
Swapchain::Recreate()
{
  // Not yet implemented
  return true;
}

std::vector<vk::Image>
Swapchain::CreateImages(const vk::Device& device,
                        const vk::SwapchainKHR& swapchain){
  TRY_EXCEPT(return device.getSwapchainImagesKHR(swapchain))
    THROW_ANY("Failed to get swapchain images!")
}

std::vector<vk::ImageView> Swapchain::CreateImageViews(
  const vk::Device& device,
  const vk::SwapchainKHR& swapchain,
  const std::vector<vk::Image>& images)
{
  std::vector<vk::ImageView> ret;
  for (const auto& image : images) {
	vk::ImageViewCreateInfo createInfo(
	  {},
	  image,
	  vk::ImageViewType::e2D,
	  m_SwapchainImageFormat,
	  { vk::ComponentSwizzle::eIdentity,
	    vk::ComponentSwizzle::eIdentity,
	    vk::ComponentSwizzle::eIdentity,
	    vk::ComponentSwizzle::eIdentity },
	  { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

	TRY_EXCEPT(ret.emplace_back(device.createImageView(createInfo)))
	THROW_ANY("Failed to create image view!")
  }

  return ret;
}
void
Swapchain::Cleanup()
{
  auto device = m_Instance.GetDevice();

  for (const auto& imageView : m_ImageViews) {
	device.destroyImageView(imageView);
  }
  device.destroySwapchainKHR(m_Swapchain);
}

} // AliusModules