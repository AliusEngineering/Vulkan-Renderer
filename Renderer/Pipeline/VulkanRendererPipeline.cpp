#include "VulkanRendererPipeline.hpp"

namespace AliusModules {

VulkanRendererPipeline::VulkanRendererPipeline()
  : m_Instance()
  , m_GraphicsPipeline(m_Instance)
  , m_CommandPipeline(m_Instance, m_GraphicsPipeline.GetSwapchain())
{
  m_MaxFrameIndex = m_GraphicsPipeline.GetSwapchain().GetMaxConcurrentFrames();
}

void
VulkanRendererPipeline::Shutdown()
{
  m_CommandPipeline.Cleanup();
  m_GraphicsPipeline.Cleanup();
  m_Instance.Cleanup();
}

void
VulkanRendererPipeline::BeginFrame()
{
  m_GraphicsPipeline.AcquireImage(m_FrameIndex);
  m_CommandPipeline.ResetBuffer(m_FrameIndex);
  m_CommandPipeline.BeginBuffer(m_FrameIndex,
                                m_GraphicsPipeline.GetRenderPass(),
                                vk::SubpassContents::eInline,
                                m_GraphicsPipeline.GetFramebuffer(m_FrameIndex),
                                m_GraphicsPipeline.GetPipeline(),
                                vk::PipelineBindPoint::eGraphics);
}

void
VulkanRendererPipeline::Draw(uint32_t vCount,
                             uint32_t iCount,
                             uint32_t firstVertex,
                             uint32_t firstInstance)
{
  m_CommandPipeline.Draw(
    m_FrameIndex, vCount, firstVertex, iCount, firstInstance);
}

void
VulkanRendererPipeline::EndFrame()
{
  auto buffer = m_CommandPipeline.EndBuffer(m_FrameIndex);
  m_GraphicsPipeline.SubmitToComputeQueue(m_FrameIndex, buffer);
  m_GraphicsPipeline.PresentQueue(m_FrameIndex);

  m_FrameIndex = (m_FrameIndex + 1) % m_MaxFrameIndex;
}

} // AliusModules