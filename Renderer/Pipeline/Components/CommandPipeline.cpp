#include "CommandPipeline.hpp"

namespace AliusModules {

CommandPipeline::CommandPipeline(const Instance& instance,
                                 const Swapchain& swapchain)
  : m_Instance(instance)
  , m_Swapchain(swapchain)
{
  m_Pool = CreateCommandPool();
  m_Buffers = AllocateCommandBuffers();
}

vk::CommandPool
CommandPipeline::CreateCommandPool()
{
  vk::CommandPoolCreateInfo createInfo{
	{ vk::CommandPoolCreateFlagBits::eResetCommandBuffer },
	m_Instance.GetQueueFamilies().Compute
  };

  TRY_EXCEPT(return m_Instance.GetDevice().createCommandPool(createInfo))
  THROW_ANY("Failed to create command pool!")
}

std::vector<vk::CommandBuffer>
CommandPipeline::AllocateCommandBuffers()
{
  vk::CommandBufferAllocateInfo allocateInfo{
	m_Pool,
	vk::CommandBufferLevel::ePrimary,
	m_Swapchain.GetMaxConcurrentFrames()
  };

  TRY_EXCEPT(return m_Instance.GetDevice().allocateCommandBuffers(allocateInfo))
  THROW_ANY("Failed to allocate command buffers!")
}

void
CommandPipeline::Cleanup()
{
  auto device = m_Instance.GetDevice();

  device.waitIdle();

  device.destroyCommandPool(m_Pool);
}
void
CommandPipeline::BeginBuffer(uint32_t index,
                             const vk::RenderPass& renderPass,
                             const vk::SubpassContents& subpassContents,
                             const vk::Framebuffer& framebuffer,
                             const vk::Pipeline& pipeline,
                             const vk::PipelineBindPoint& pipelineBindPoint)
{
  vk::CommandBufferBeginInfo beginInfo{};
  TRY_EXCEPT(m_Buffers.at(index).begin(beginInfo))
  THROW_ANY("Failed to begin command buffer!")

  vk::ClearValue clearValue{ { std::array<float, 4>{
	0.0f, 0.0f, 0.0f, 1.0f } } };
  vk::RenderPassBeginInfo renderPassBeginInfo{ renderPass,
	                                           framebuffer,
	                                           { { 0, 0 },
	                                             m_Swapchain.GetMeta().Extent },
	                                           1,
	                                           &clearValue };

  m_Buffers.at(index).beginRenderPass(renderPassBeginInfo, subpassContents);
  m_Buffers.at(index).bindPipeline(pipelineBindPoint, pipeline);

  m_Buffers.at(index).setViewport(
    0,
    vk::Viewport{ 0,
                  0,
                  static_cast<float>(m_Swapchain.GetMeta().Extent.width),
                  static_cast<float>(m_Swapchain.GetMeta().Extent.height),
                  0.0f,
                  1.0f });

  m_Buffers.at(index).setScissor(
    0, vk::Rect2D{ { 0, 0 }, m_Swapchain.GetMeta().Extent });
}

void
CommandPipeline::ResetBuffer(uint32_t index){
  TRY_EXCEPT(m_Buffers.at(index).reset())
    WARN_ANY("Failed to reset command buffer at index " + std::to_string(index))
}

vk::CommandBuffer CommandPipeline::EndBuffer(uint32_t index)
{
  m_Buffers.at(index).endRenderPass();
  TRY_EXCEPT(m_Buffers.at(index).end(); return m_Buffers.at(index))
  THROW_ANY("Failed to end command buffer!")
}
void
CommandPipeline::Draw(uint32_t index,
                      uint32_t vertexCount,
                      uint32_t firstVertex,
                      uint32_t instanceCount,
                      uint32_t firstInstance)
{
  m_Buffers.at(index).draw(
    vertexCount, instanceCount, firstVertex, firstInstance);
}

} // AliusModules