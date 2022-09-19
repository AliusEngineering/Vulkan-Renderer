#pragma once

#include "VulkanRendererPch.hpp"

#include "Pipeline/Components/Instance.hpp"
#include "Pipeline/Components/Swapchain.hpp"

#include "Pipeline/Components/CommandPipeline.hpp"
#include "Pipeline/Components/GraphicsPipeline.hpp"

namespace AliusModules {

class VulkanRendererPipeline
{
public:
  VulkanRendererPipeline();

  void BeginFrame();

  void Draw(uint32_t vCount,
            uint32_t iCount,
            uint32_t firstVertex,
            uint32_t firstInstance);

  void EndFrame();

  void Shutdown();

private:
  Instance m_Instance;
  GraphicsPipeline m_GraphicsPipeline;
  CommandPipeline m_CommandPipeline;

private:
  uint32_t m_FrameIndex = 0;
  uint32_t m_MaxFrameIndex = 0;
};

} // AliusModules
