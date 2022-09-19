#pragma once

#include "RendererBase/Renderer.hpp"

#include "Pipeline/VulkanRendererPipeline.hpp"

#include "Window/VulkanWindow.hpp"

namespace AliusModules {

class VulkanRenderer : public Alius::Renderer
{
public:
  VulkanRenderer(size_t width, size_t height, const char* title);
  ~VulkanRenderer() override;

  void BeginFrame() override;

  void Draw(uint32_t vertexCount,
            uint32_t firstVertex,
            uint32_t instanceCount,
            uint32_t firstInstance) override;

  void EndFrame() override;

private:
  VulkanWindow m_Window;
  VulkanRendererPipeline m_RendererPipeline;

private:
  // Module registration
  [[maybe_unused]] static bool appendCall;
};

} // AliusModules
