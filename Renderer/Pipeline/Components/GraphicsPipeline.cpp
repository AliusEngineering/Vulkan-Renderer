#include "GraphicsPipeline.hpp"

#include <utility>

namespace AliusModules {

GraphicsPipeline::GraphicsPipeline(Ref<Instance> instance)
  : m_Instance(std::move(instance))
  , m_Swapchain(std::make_shared<Swapchain>(m_Instance))
{
  auto& device = m_Instance->GetDevice();

  m_Shaders.emplace_back(device,
                         "Resources/Shaders/WorldObject/vertex.glsl",
                         vk::ShaderStageFlagBits::eVertex);

  m_Shaders.emplace_back(device,
                         "Resources/Shaders/WorldObject/fragment.glsl",
                         vk::ShaderStageFlagBits::eFragment);

  ConstructPipelineBakedInState();
  ConstructRenderPassState();

  m_Layout = CreatePipelineLayout();
  m_RenderPass = CreateRenderPass();

  m_Framebuffers = CreateFramebuffers();

  m_Pipeline = CreatePipeline();

  // Create synchronizers
  for (int i = 0; i < m_Swapchain->GetMaxConcurrentFrames(); i++) {
	m_ImageSemaphores.emplace_back(CreateSemaphore());
	m_RenderSemaphores.emplace_back(CreateSemaphore());

	m_RenderFences.emplace_back(CreateFence());
  }
}

uint32_t GraphicsPipeline::AcquireImage(uint32_t index)
{
  if (index >= m_Swapchain->GetMaxConcurrentFrames()) {
	SQD_WARN("Bad image index: tried to acquire index {} while maximum "
	         "available is {}",
	         index,
	         m_Swapchain->GetMaxConcurrentFrames() - 1);
	return UINT32_MAX;
  }

  auto& device = m_Instance->GetDevice();

  (void)device.waitForFences(
    m_RenderFences.at(index), VK_TRUE, c_SynchronizersTimeout);

  auto acquisitionRes = device.acquireNextImageKHR(m_Swapchain->GetSwapchain(),
                                                   c_SynchronizersTimeout,
                                                   m_ImageSemaphores.at(index),
                                                   nullptr,
                                                   &m_CurrentImageIndex);

  switch (acquisitionRes) {
	case vk::Result::eSuccess:
	  break;
	case vk::Result::eSuboptimalKHR:
	case vk::Result::eErrorOutOfDateKHR: {
	  RecreateSwapchain();
	  return UINT32_MAX;
	}
	default: {
	  SQD_WARN("Failed to acquire image from swapchain!");
	  return UINT32_MAX;
	}
  }

  device.resetFences(m_RenderFences.at(index));
  return m_CurrentImageIndex;
}

bool GraphicsPipeline::SubmitToComputeQueue(
  uint32_t index,
  const vk::CommandBuffer& commandBuffer)
{
  vk::PipelineStageFlags waitStages{
	vk::PipelineStageFlagBits::eColorAttachmentOutput
  };

  vk::SubmitInfo submitInfo{ m_ImageSemaphores.at(index),
	                         waitStages,
	                         commandBuffer,
	                         m_RenderSemaphores.at(index) };

  TRY_EXCEPT(
    m_Swapchain->GetComputeQueue().submit(submitInfo, m_RenderFences.at(index)))
  THROW_ANY("Failed to submit queue!")

  return true;
}

bool GraphicsPipeline::PresentQueue(uint32_t frameIndex, uint32_t imageIndex)
{
  auto swapchain = m_Swapchain->GetSwapchain();

  vk::PresentInfoKHR presentInfo{ m_RenderSemaphores.at(frameIndex),
	                              swapchain,
	                              imageIndex };

  auto presentationRes = m_Swapchain->GetPresentQueue().presentKHR(presentInfo);

  switch (presentationRes) {
	case vk::Result::eSuccess:
	  break;
	case vk::Result::eSuboptimalKHR:
	case vk::Result::eErrorOutOfDateKHR: {
	  RecreateSwapchain();
	  return false;
	}
	default: {
	  SQD_WARN("Failed to acquire image from swapchain!");
	  return false;
	}
  }

  return true;
}

void GraphicsPipeline::ConstructPipelineBakedInState()
{
  m_PipelineBakedInState.DynamicState = { {},
	                                      static_cast<uint32_t>(
	                                        c_PipelineDynamicStates.size()),
	                                      c_PipelineDynamicStates.data() };

  m_PipelineBakedInState.VertexInput = { {},
	                                     c_WorldObjectVertexInputBindings,
	                                     c_WorldObjectVertexAttributes };

  m_PipelineBakedInState.InputAssembly = { {},
	                                       vk::PrimitiveTopology::eTriangleList,
	                                       VK_FALSE };

  m_PipelineBakedInState.ViewportState = { {}, 1, nullptr, 1, nullptr };

  m_PipelineBakedInState.Rasterization = { {},
	                                       VK_FALSE,
	                                       VK_FALSE,
	                                       vk::PolygonMode::eFill,
	                                       vk::CullModeFlagBits::eNone,
	                                       vk::FrontFace::eCounterClockwise,
	                                       VK_FALSE,
	                                       0.0f,
	                                       0.0f,
	                                       0.0f,
	                                       1.0f };

  m_PipelineBakedInState.Multisample = { {},
	                                     vk::SampleCountFlagBits::e1,
	                                     VK_FALSE };

  m_PipelineBakedInState.ColorBlendAttachment = {
	VK_TRUE,
	vk::BlendFactor::eSrcAlpha,
	vk::BlendFactor::eOneMinusSrcAlpha,
	vk::BlendOp::eAdd,
	vk::BlendFactor::eOne,
	vk::BlendFactor::eZero,
	vk::BlendOp::eAdd,
	vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
	  vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
  };

  m_PipelineBakedInState.ColorBlend = {
	{},
	VK_FALSE,
	vk::LogicOp::eClear,
	1,
	&m_PipelineBakedInState.ColorBlendAttachment,
	{ 0.0f, 0.0f, 0.0f, 0.0f }
  };

  m_PipelineBakedInState.Layout = { {}, nullptr, nullptr };
}

void GraphicsPipeline::ConstructRenderPassState()
{
  m_RenderPassState.AttachmentDescription = { {},
	                                          m_Swapchain->ImageFormat,
	                                          vk::SampleCountFlagBits::e1,
	                                          vk::AttachmentLoadOp::eClear,
	                                          vk::AttachmentStoreOp::eStore,
	                                          vk::AttachmentLoadOp::eDontCare,
	                                          vk::AttachmentStoreOp::eDontCare,
	                                          vk::ImageLayout::eUndefined,
	                                          vk::ImageLayout::ePresentSrcKHR };

  m_RenderPassState.AttachmentReference = {
	0, vk::ImageLayout::eColorAttachmentOptimal
  };

  m_RenderPassState.SubpassDependency = {
	VK_SUBPASS_EXTERNAL,
	0,
	vk::PipelineStageFlagBits::eColorAttachmentOutput,
	vk::PipelineStageFlagBits::eColorAttachmentOutput,
	vk::AccessFlagBits::eNone,
	vk::AccessFlagBits::eColorAttachmentWrite
  };

  m_RenderPassState.SubpassDescription = {
	{}, vk::PipelineBindPoint::eGraphics,      0, nullptr,
	1,  &m_RenderPassState.AttachmentReference
  };
}

vk::PipelineLayout GraphicsPipeline::CreatePipelineLayout(){
  TRY_EXCEPT(return m_Instance->GetDevice().createPipelineLayout(
    m_PipelineBakedInState.Layout))
    THROW_ANY("Failed to create pipeline layout!")
}

vk::RenderPass GraphicsPipeline::CreateRenderPass()
{
  vk::RenderPassCreateInfo createInfo{ {},
	                                   1,
	                                   &m_RenderPassState.AttachmentDescription,
	                                   1,
	                                   &m_RenderPassState.SubpassDescription,
	                                   1,
	                                   &m_RenderPassState.SubpassDependency };

  TRY_EXCEPT(return m_Instance->GetDevice().createRenderPass(createInfo))
  THROW_ANY("Failed to create render pass!")
}

vk::Pipeline GraphicsPipeline::CreatePipeline()
{
  std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
  for (const auto& shader : m_Shaders) {
	if (shader.IsOK())
	  shaderStages.emplace_back(shader.GetStageCreateInfo());
  }

  vk::GraphicsPipelineCreateInfo createInfo{
	{},
	static_cast<uint32_t>(shaderStages.size()),
	shaderStages.data(),
	&m_PipelineBakedInState.VertexInput,
	&m_PipelineBakedInState.InputAssembly,
	nullptr,
	&m_PipelineBakedInState.ViewportState,
	&m_PipelineBakedInState.Rasterization,
	&m_PipelineBakedInState.Multisample,
	&m_PipelineBakedInState.DepthStencil,
	&m_PipelineBakedInState.ColorBlend,
	&m_PipelineBakedInState.DynamicState,
	m_Layout,
	m_RenderPass,
	0
  };

  TRY_EXCEPT(return m_Instance->GetDevice()
               .createGraphicsPipeline(nullptr, createInfo)
               .value)
  THROW_ANY("Failed to create graphics pipeline!")
}

std::vector<vk::Framebuffer> GraphicsPipeline::CreateFramebuffers()
{
  std::vector<vk::Framebuffer> ret;
  for (const auto& imageView : m_Swapchain->GetImageViews()) {
	vk::FramebufferCreateInfo createInfo{ {},
	                                      m_RenderPass,
	                                      imageView,
	                                      m_Swapchain->Extent.width,
	                                      m_Swapchain->Extent.height,
	                                      1 };

	TRY_EXCEPT(
	  ret.emplace_back(m_Instance->GetDevice().createFramebuffer(createInfo)))
	THROW_ANY("Failed to create framebuffers!")
  }

  return ret;
}

void GraphicsPipeline::RecreateFramebuffers()
{
  auto& device = m_Instance->GetDevice();

  device.waitIdle();

  for (const auto& fb : m_Framebuffers) {
	device.destroyFramebuffer(fb);
  }

  m_Framebuffers = CreateFramebuffers();
}

vk::Semaphore GraphicsPipeline::CreateSemaphore()
{
  vk::SemaphoreCreateInfo createInfo{};
  TRY_EXCEPT(return m_Instance->GetDevice().createSemaphore(createInfo))
  THROW_ANY("Failed to create semaphore!")
}

vk::Fence GraphicsPipeline::CreateFence()
{
  vk::FenceCreateInfo createInfo{ vk::FenceCreateFlagBits::eSignaled };
  TRY_EXCEPT(return m_Instance->GetDevice().createFence(createInfo))
  THROW_ANY("Failed to create semaphore!")
}

void GraphicsPipeline::RecreateSwapchain()
{
  m_Swapchain->Recreate();
  RecreateFramebuffers();
}

void GraphicsPipeline::Cleanup()
{
  auto& device = m_Instance->GetDevice();

  for (int i = 0; i < m_Swapchain->GetMaxConcurrentFrames(); i++) {
	device.destroySemaphore(m_RenderSemaphores.at(i));
	device.destroySemaphore(m_ImageSemaphores.at(i));
	device.destroyFence(m_RenderFences.at(i));
  }

  for (const auto& fb : m_Framebuffers) {
	device.destroyFramebuffer(fb);
  }

  device.destroyPipeline(m_Pipeline);
  device.destroyPipelineLayout(m_Layout);
  device.destroyRenderPass(m_RenderPass);
  for (const auto& shader : m_Shaders) {
	device.destroyShaderModule(shader.GetModule());
  }

  m_Swapchain->Cleanup();
}

} // AliusModules