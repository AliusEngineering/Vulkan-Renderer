#include "Swapchain.hpp"

#include <utility>

namespace AliusModules {

Swapchain::Swapchain(Ref<Instance> instance)
  : m_Instance(std::move(instance))
{
  auto& device = m_Instance->GetDevice();

  m_Swapchain = CreateSwapchain();

  m_ComputeQueue =
    SwapchainHelpers::GetQueue(device, m_Instance->GetQueueFamilies().Compute);
  m_PresentQueue =
    SwapchainHelpers::GetQueue(device, m_Instance->GetQueueFamilies().Present);

  m_Images = CreateImages(device, m_Swapchain);
  m_ImageViews = CreateImageViews(device, m_Swapchain, m_Images);
}

vk::SwapchainKHR Swapchain::CreateSwapchain(
  const vk::SwapchainKHR& oldSwapchain)
{
  auto supportDetails = SwapchainHelpers::QuerySupportDetails(
    m_Instance->GetPhysicalDevice(), m_Instance->GetSurface());

  if (supportDetails.PresentModes.empty() ||
      supportDetails.SurfaceFormats.empty()) {
	SQD_ERR("Selected physical device does not feature sufficient swapchain "
	        "support!");
	throw std::runtime_error("Selected physical device does not feature "
	                         "sufficient swapchain support!");
  }

  auto format =
    SwapchainHelpers::PickSurfaceFormat(supportDetails.SurfaceFormats);
  auto presentMode =
    SwapchainHelpers::PickPresentMode(supportDetails.PresentModes);
  m_MaxConcurrentFrames =
    SwapchainHelpers::PickOptimalConcurrentFrames(presentMode);
  auto extent = SwapchainHelpers::PickExtend2D(supportDetails.SurfaceCaps);

  ImageFormat = format.format;
  Extent = extent;

  uint32_t imageCount = std::clamp(supportDetails.SurfaceCaps.minImageCount + 3,
                                   supportDetails.SurfaceCaps.minImageCount,
                                   supportDetails.SurfaceCaps.maxImageCount != 0
                                     ? supportDetails.SurfaceCaps.maxImageCount
                                     : 100);

  vk::SwapchainCreateInfoKHR createInfo{
	{},
	m_Instance->GetSurface(),
	imageCount,
	ImageFormat,
	format.colorSpace,
	Extent,
	1,
	vk::ImageUsageFlagBits::eColorAttachment
  };

  auto& queueFamilies = m_Instance->GetQueueFamilies();

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

  createInfo.oldSwapchain = oldSwapchain;

  TRY_EXCEPT(return m_Instance->GetDevice().createSwapchainKHR(createInfo))
  THROW_ANY("Failed to create swapchain!")
}

void Swapchain::CleanupBeforeRecreate()
{
  auto& device = m_Instance->GetDevice();

  device.waitIdle();

  for (const auto& imageView : m_ImageViews) {
	device.destroyImageView(imageView);
  }
}

bool Swapchain::Recreate()
{
  auto& device = m_Instance->GetDevice();

  device.waitIdle();

  CleanupBeforeRecreate();

  m_OldSwapchain = m_Swapchain;

  m_Swapchain = CreateSwapchain(m_OldSwapchain);

  device.destroySwapchainKHR(m_OldSwapchain);

  m_Images = CreateImages(device, m_Swapchain);
  m_ImageViews = CreateImageViews(device, m_Swapchain, m_Images);

  return true;
}

std::vector<vk::Image> Swapchain::CreateImages(
  const vk::Device& device,
  const vk::SwapchainKHR& swapchain){
  TRY_EXCEPT(return device.getSwapchainImagesKHR(swapchain))
    THROW_ANY("Failed to get swapchain images!")
}

std::vector<vk::ImageView> Swapchain::CreateImageViews(
  const vk::Device& device,
  const vk::SwapchainKHR& swapchain,
  const std::vector<vk::Image>& images)
{
  std::vector<vk::ImageView> imageViews;

  for (const auto& image : images) {
	vk::ImageViewCreateInfo createInfo(
	  {},
	  image,
	  vk::ImageViewType::e2D,
	  ImageFormat,
	  { vk::ComponentSwizzle::eIdentity,
	    vk::ComponentSwizzle::eIdentity,
	    vk::ComponentSwizzle::eIdentity,
	    vk::ComponentSwizzle::eIdentity },
	  { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

	TRY_EXCEPT(imageViews.emplace_back(device.createImageView(createInfo)))
	THROW_ANY("Failed to create image view!")
  }

  return imageViews;
}
void Swapchain::Cleanup()
{
  auto& device = m_Instance->GetDevice();

  device.waitIdle();

  for (const auto& imageView : m_ImageViews) {
	device.destroyImageView(imageView);
  }
  device.destroySwapchainKHR(m_Swapchain);
}

} // AliusModules