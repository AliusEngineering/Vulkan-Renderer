#pragma once

#include "VulkanRendererPch.hpp"

#include "Instance.hpp"
#include "Swapchain.hpp"

namespace AliusModules {

class CommandPipeline
{
public:
  explicit CommandPipeline(Instance* instance, Swapchain* swapchain);

  void BeginBuffer(uint32_t index,
                   const vk::RenderPass& renderPass,
                   const vk::SubpassContents& subpassContents,
                   const vk::Framebuffer& framebuffer,
                   const vk::Pipeline& pipeline,
                   const vk::PipelineBindPoint& pipelineBindPoint);
  void ResetBuffer(uint32_t index);

  vk::CommandBuffer EndBuffer(uint32_t index);

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
  Instance* m_Instance;
  Swapchain* m_Swapchain;

  vk::CommandPool m_Pool;
  std::vector<vk::CommandBuffer> m_Buffers;
};

} // AliusModules
