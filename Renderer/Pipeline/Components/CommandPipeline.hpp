#pragma once

#include "VulkanRendererPch.hpp"

#include "Instance.hpp"
#include "Swapchain.hpp"

namespace AliusModules {

class CommandPipeline
{
public:
  explicit CommandPipeline(const Instance& instance,
                           const Swapchain& swapchain);

  void BeginBuffer(uint32_t index,
                   const vk::RenderPass& renderPass,
                   const vk::SubpassContents& subpassContents,
                   const vk::Framebuffer& framebuffer,
                   const vk::Pipeline& pipeline,
                   const vk::PipelineBindPoint& pipelineBindPoint);
  void ResetBuffer(uint32_t index);

  /**
   * Ends the command buffer and returns it back for later use.
   * @param index Buffer index
   * @return Recorded command buffer
   */
  vk::CommandBuffer EndBuffer(uint32_t index);

  /**
   * Draw vertices and instances by amount
   * @param index Index of command buffer to operate on.
   * @param vertexCount Amount to vertices to draw.
   * @param firstVertex First vertex to start from.
   * @param instanceCount Amount of instances to draw.
   * @param firstInstance First instance to start from.
   */
  void Draw(uint32_t index,
            uint32_t vertexCount,
            uint32_t firstVertex,
            uint32_t instanceCount = 1,
            uint32_t firstInstance = 0);

  void Cleanup();

private:
  vk::CommandPool CreateCommandPool();
  std::vector<vk::CommandBuffer> AllocateCommandBuffers();

private:
  Instance m_Instance;
  Swapchain m_Swapchain;

  vk::CommandPool m_Pool;
  std::vector<vk::CommandBuffer> m_Buffers;
};

} // AliusModules
