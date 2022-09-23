#include "VulkanRendererPipeline.hpp"

namespace AliusModules {

VulkanRendererPipeline::VulkanRendererPipeline()
  : m_Instance(new Instance())
  , m_GraphicsPipeline(new GraphicsPipeline(m_Instance))
  , m_CommandPipeline(
      new CommandPipeline(m_Instance, m_GraphicsPipeline->GetSwapchain()))
{
  m_MaxFrameIndex =
    m_GraphicsPipeline->GetSwapchain()->GetMaxConcurrentFrames();
}

void
VulkanRendererPipeline::Shutdown()
{
  m_Instance->GetDevice().waitIdle();

  m_CommandPipeline->Cleanup();
  m_GraphicsPipeline->Cleanup();
  m_Instance->Cleanup();

  // deallocate all heap memory
  delete m_CommandPipeline;
  delete m_GraphicsPipeline;
  delete m_Instance;
}

void
VulkanRendererPipeline::BeginFrame()
{
  m_ImageIndex = m_GraphicsPipeline->AcquireImage(m_FrameIndex);
  if (m_ImageIndex == UINT32_MAX) {
	m_FrameIndex = 0;
	m_FrameBegun = false;
	return;
  }

  m_FrameBegun = true;

  m_CommandPipeline->ResetBuffer(m_FrameIndex);
  m_CommandPipeline->BeginBuffer(
    m_FrameIndex,
    m_GraphicsPipeline->GetRenderPass(),
    vk::SubpassContents::eInline,
    m_GraphicsPipeline->GetFramebuffer(m_ImageIndex),
    m_GraphicsPipeline->GetPipeline(),
    vk::PipelineBindPoint::eGraphics);
}

void
VulkanRendererPipeline::Draw(uint32_t vCount,
                             uint32_t iCount,
                             uint32_t firstVertex,
                             uint32_t firstInstance)
{
  if (!m_FrameBegun)
	return;

  m_CommandPipeline->Draw(
    m_FrameIndex, vCount, firstVertex, iCount, firstInstance);
}

void
VulkanRendererPipeline::EndFrame()
{
  if (!m_FrameBegun) {
	m_FrameIndex = 0;
	return;
  }

  auto buffer = m_CommandPipeline->EndBuffer(m_FrameIndex);
  m_GraphicsPipeline->SubmitToComputeQueue(m_FrameIndex, buffer);
  m_GraphicsPipeline->PresentQueue(m_FrameIndex, m_ImageIndex);

  m_FrameIndex = (m_FrameIndex + 1) % m_MaxFrameIndex;

  m_FrameBegun = false;
}

} // AliusModules