#include "CommandPipeline.hpp"

#include "thread"

namespace AliusModules {

CommandPipeline::CommandPipeline(Instance* instance, Swapchain* swapchain)
  : m_Instance(instance)
  , m_Swapchain(swapchain)
{
  m_Pool = CreateCommandPool(m_Instance->GetQueueFamilies().Compute);
  m_Buffers =
    AllocateCommandBuffers(m_Pool, m_Swapchain->GetMaxConcurrentFrames());

  m_AvailableThreads = std::thread::hardware_concurrency();

  SQD_LOG("Found {} available CPU threads (if the number is zero, concurrency "
          "isn't well defined; 1 buffer will be created)! Creating the same "
          "amount of transfer command buffers for concurrent renderer object "
          "submission.",
          m_AvailableThreads);

  m_TransferPool = CreateCommandPool(m_Instance->GetQueueFamilies().Compute);
  m_TransferBuffers = AllocateCommandBuffers(
    m_TransferPool, m_AvailableThreads ? m_AvailableThreads : 1);
}

vk::Buffer CommandPipeline::CreateBuffer(vk::DeviceSize size,
                                         vk::BufferUsageFlags usageFlags)
{
  vk::BufferCreateInfo createInfo{ {}, size, usageFlags };
  TRY_EXCEPT(return m_Instance->GetDevice().createBuffer(createInfo))
  THROW_ANY("Failed to create new buffer!")
}

vk::DeviceMemory CommandPipeline::AllocateDeviceMemory(
  const vk::Buffer& buffer,
  vk::MemoryPropertyFlags memoryProperties)
{
  auto device = m_Instance->GetDevice();

  auto memoryRequirements = device.getBufferMemoryRequirements(buffer);

  vk::MemoryAllocateInfo allocateInfo{ memoryRequirements.size,
	                                   CommandPipelineHelpers::FindMemoryType(
	                                     m_Instance->GetPhysicalDevice(),
	                                     memoryRequirements.memoryTypeBits,
	                                     memoryProperties) };

  auto allocatedMemory = device.allocateMemory(allocateInfo);
  device.bindBufferMemory(buffer, allocatedMemory, 0);

  TRY_EXCEPT(return allocatedMemory)
  THROW_ANY("Failed to allocate device memory!")
}

void CommandPipeline::CopyToDeviceMemory(const vk::DeviceMemory& deviceMemory,
                                         vk::DeviceSize memoryMappingOffset,
                                         void* data,
                                         vk::DeviceSize dataSize)
{
  auto device = m_Instance->GetDevice();

  try {
	void* memoryMapping =
	  device.mapMemory(deviceMemory, memoryMappingOffset, dataSize);

	memcpy(memoryMapping, data, dataSize);

	device.unmapMemory(deviceMemory);
  } catch (...) {
	SQD_WARN("Failed to copy to device memory!");
  }
}

void CommandPipeline::CopyBuffers(const vk::Buffer& source,
                                  const vk::Buffer& destination,
                                  vk::DeviceSize bufferSize)
{
  auto device = m_Instance->GetDevice();
  auto commandBuffer = m_TransferBuffers[m_TransferBufferIndex];
  auto queue = m_Swapchain->GetComputeQueue();

  commandBuffer.reset();

  vk::CommandBufferBeginInfo transferBufferBeginInfo{
	vk::CommandBufferUsageFlagBits::eOneTimeSubmit
  };

  commandBuffer.begin(transferBufferBeginInfo);

  {
	vk::BufferCopy bufferCopyRegion{ {}, {}, bufferSize };
	commandBuffer.copyBuffer(source, destination, bufferCopyRegion);
  }

  commandBuffer.end();

  vk::SubmitInfo bufferSubmitInfo{};
  bufferSubmitInfo.setCommandBuffers(commandBuffer);

  queue.submit(bufferSubmitInfo);
  queue.waitIdle();
}

vk::CommandPool CommandPipeline::CreateCommandPool(uint32_t queueIndexFamily)
{
  vk::CommandPoolCreateInfo createInfo{
	{ vk::CommandPoolCreateFlagBits::eResetCommandBuffer }, queueIndexFamily
  };

  TRY_EXCEPT(return m_Instance->GetDevice().createCommandPool(createInfo))
  THROW_ANY("Failed to create command pool!")
}

std::vector<vk::CommandBuffer> CommandPipeline::AllocateCommandBuffers(
  const vk::CommandPool& pool,
  uint32_t count)
{
  vk::CommandBufferAllocateInfo allocateInfo{ pool,
	                                          vk::CommandBufferLevel::ePrimary,
	                                          count };

  TRY_EXCEPT(
    return m_Instance->GetDevice().allocateCommandBuffers(allocateInfo))
  THROW_ANY("Failed to allocate command buffers!")
}

void CommandPipeline::BeginBuffer(
  uint32_t index,
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

  vk::RenderPassBeginInfo renderPassBeginInfo{
	renderPass, framebuffer, { { 0, 0 }, m_Swapchain->Extent }, 1, &clearValue
  };

  m_Buffers.at(index).beginRenderPass(renderPassBeginInfo, subpassContents);
  m_Buffers.at(index).bindPipeline(pipelineBindPoint, pipeline);

  m_Buffers.at(index).setViewport(
    0,
    vk::Viewport{ 0,
                  0,
                  static_cast<float>(m_Swapchain->Extent.width),
                  static_cast<float>(m_Swapchain->Extent.height),
                  0.0f,
                  1.0f });

  m_Buffers.at(index).setScissor(0,
                                 vk::Rect2D{ { 0, 0 }, m_Swapchain->Extent });
}

void CommandPipeline::ResetBuffer(uint32_t index)
{
  TRY_EXCEPT(m_Buffers.at(index).reset())
  WARN_ANY("Failed to reset command buffer at index " + std::to_string(index))
}

void CommandPipeline::BindVertexBuffer(uint32_t index, const vk::Buffer& buffer)
{
  vk::DeviceSize offsets[]{ 0 };
  m_Buffers.at(index).bindVertexBuffers(0, buffer, offsets);
}

void CommandPipeline::BindIndexBuffer(uint32_t index, const vk::Buffer& buffer)
{
  m_Buffers.at(index).bindIndexBuffer(buffer, 0, vk::IndexType::eUint32);
}

vk::CommandBuffer CommandPipeline::EndBuffer(uint32_t index)
{
  m_Buffers.at(index).endRenderPass();
  TRY_EXCEPT(m_Buffers.at(index).end(); return m_Buffers.at(index))
  THROW_ANY("Failed to end command buffer!")
}

void CommandPipeline::Draw(uint32_t index,
                           uint32_t vertexCount,
                           uint32_t firstVertex,
                           uint32_t instanceCount,
                           uint32_t firstInstance)
{
  m_Buffers.at(index).draw(
    vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandPipeline::DrawIndexed(uint32_t index,
                                  uint32_t indicesCount,
                                  uint32_t instanceCount,
                                  uint32_t firstInstance,
                                  int vertexOffset)
{
  m_Buffers.at(index).drawIndexed(
    indicesCount, instanceCount, 0, vertexOffset, firstInstance);
}

void CommandPipeline::Cleanup()
{
  auto device = m_Instance->GetDevice();

  device.destroyCommandPool(m_Pool);
  device.destroyCommandPool(m_TransferPool);
}

} // AliusModules