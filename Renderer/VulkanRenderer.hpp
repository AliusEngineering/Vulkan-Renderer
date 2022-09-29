#pragma once

#include "RendererBase/Renderer.hpp"

#include "Pipeline/VulkanRendererPipeline.hpp"

#include "Window.hpp"

namespace AliusModules {

class VulkanRenderer : public Alius::Renderer
{
public:
  VulkanRenderer(size_t width, size_t height, const char* title);
  ~VulkanRenderer() override;

  Ref<Alius::Window> GetWindow() const override;

  void BeginFrame() override;

  void Draw(Ref<Alius::RendererObjectBase> object) override;

  void EndFrame() override;

private:
  void CreateObjectImpl(Ref<Alius::RendererObjectBase> object) override;
  void DestroyObjectImpl(Ref<Alius::RendererObjectBase> object) override;

private:
  std::shared_ptr<Alius::Window> m_Window;
  VulkanRendererPipeline m_RendererPipeline;

  std::vector<Ref<Alius::RendererObjectBase>> m_ObjectStorage;

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
