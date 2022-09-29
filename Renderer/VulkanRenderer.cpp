#include "VulkanRenderer.hpp"

#include "Window/VulkanWindow.hpp"

namespace AliusModules {

VulkanRenderer::VulkanRenderer(size_t width, size_t height, const char* title)
  : m_Window(std::make_shared<VulkanWindow>(width, height, title))
  , m_RendererPipeline()
{
}

VulkanRenderer::~VulkanRenderer()
{
  for (const auto& object : m_ObjectStorage) {
	auto implData =
	  std::dynamic_pointer_cast<Alius::VkObjectData>(object->GetImplData());
	if (implData == nullptr)
	  continue;

	m_RendererPipeline.DestroyRendererObj(implData);
  }

  m_RendererPipeline.Shutdown();
}

std::shared_ptr<Alius::Window> VulkanRenderer::GetWindow() const
{
  return m_Window;
}

void VulkanRenderer::CreateObjectImpl(
  std::shared_ptr<Alius::RendererObjectBase> object)
{
  m_ObjectStorage.push_back(object);
  m_RendererPipeline.CreateRendererObj(object);
}

void VulkanRenderer::DestroyObjectImpl(Ref<Alius::RendererObjectBase> object)
{
  auto implData =
    std::dynamic_pointer_cast<Alius::VkObjectData>(object->GetImplData());
  if (implData == nullptr)
	return;

  m_RendererPipeline.DestroyRendererObj(implData);
}

void VulkanRenderer::BeginFrame()
{
  if (m_Window->IsActive())
	m_RendererPipeline.BeginFrame();
}

void VulkanRenderer::Draw(std::shared_ptr<Alius::RendererObjectBase> object)
{
  m_RendererPipeline.Draw(object);
}

void VulkanRenderer::EndFrame()
{
  if (m_Window->IsActive())
	m_RendererPipeline.EndFrame();
}

} // AliusModules
