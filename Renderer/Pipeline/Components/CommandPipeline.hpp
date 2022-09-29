#pragma once

#include "VulkanRendererPch.hpp"

#include "Instance.hpp"
#include "Swapchain.hpp"

namespace AliusModules {

class CommandPipeline
{
public:
  explicit CommandPipeline(Instance* instance, Swapchain* swapchain);

  vk::Buffer CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usageFlags);

  vk::DeviceMemory AllocateDeviceMemory(
    const vk::Buffer& buffer,
    vk::MemoryPropertyFlags memoryProperties);

  void CopyToDeviceMemory(const vk::DeviceMemory& deviceMemory,
                          vk::DeviceSize memoryMappingOffset,
                          void* data,
                          vk::DeviceSize dataSize);

  void CopyBuffers(const vk::Buffer& source,
                   const vk::Buffer& destination,
                   vk::DeviceSize bufferSize);

  void BeginBuffer(uint32_t index,
                   const vk::RenderPass& renderPass,
                   const vk::SubpassContents& subpassContents,
                   const vk::Framebuffer& framebuffer,
                   const vk::Pipeline& pipeline,
                   const vk::PipelineBindPoint& pipelineBindPoint);
  void ResetBuffer(uint32_t index);

  void BindVertexBuffer(uint32_t index, const vk::Buffer& buffer);
  void BindIndexBuffer(uint32_t index, const vk::Buffer& buffer);

  vk::CommandBuffer EndBuffer(uint32_t index);

  void Draw(uint32_t index,
            uint32_t vertexCount,
            uint32_t firstVertex,
            uint32_t instanceCount = 1,
            uint32_t firstInstance = 0);

  void DrawIndexed(uint32_t index,
                   uint32_t indicesCount,
                   uint32_t instanceCount = 1,
                   uint32_t firstInstance = 0,
                   int vertexOffset = 0);

  void Cleanup();

private:
  vk::CommandPool CreateCommandPool(uint32_t queueIndexFamily);
  std::vector<vk::CommandBuffer> AllocateCommandBuffers(
    const vk::CommandPool& pool,
    uint32_t count);

private:
  Instance* m_Instance;
  Swapchain* m_Swapchain;

  vk::CommandPool m_Pool, m_TransferPool;
  std::vector<vk::CommandBuffer> m_Buffers, m_TransferBuffers;

  uint32_t m_TransferBufferIndex = 0;
  ulong m_AvailableThreads = 0;
};

} // AliusModules
