#pragma once

#include "RendererBase/Renderer.hpp"

#include "Pipeline/VulkanRendererPipeline.hpp"

#include "Window.hpp"

namespace AliusModules {

class VulkanRenderer : public Alius::Renderer
{
public:
  VulkanRenderer(const Alius::RendererSpec& spec);
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
  inline static bool s_Registration = ([]() {
	Alius::Renderer::ModuleSpec moduleSpec{
	  [](const Alius::RendererSpec& spec) -> Ref<Alius::Renderer> {
	    return std::make_shared<VulkanRenderer>(spec);
	  }
	};

	Alius::Renderer::s_RendererModules.insert(
	  std::make_pair("AlsVkRenderer", moduleSpec));

	return true;
  })();
};

} // AliusModules
