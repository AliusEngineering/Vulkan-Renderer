#pragma once

#include "RendererBase/Renderer.hpp"

#include "Pipeline/VulkanRendererPipeline.hpp"

#include "Window/VulkanWindow.hpp"
#include "WindowBase/Window.hpp"

namespace AliusModules {

class VulkanRenderer : public Alius::Renderer
{
public:
  VulkanRenderer(size_t width, size_t height, const char* title);
  ~VulkanRenderer() override;

  std::shared_ptr<Alius::Window> GetWindow() const override
  {
	return std::make_shared<VulkanWindow>(m_Window);
  };

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
  [[maybe_unused]] inline static bool s_Registration = ([]() {
	Alius::Renderer::s_RendererModules.insert(
	  { "AlsVkRenderer", [](size_t width, size_t height, const char* title) {
	     return std::make_shared<VulkanRenderer>(width, height, title);
	   } });
	return true;
  })();
};

} // AliusModules
