#include "VulkanRenderer.hpp"

namespace AliusModules {

VulkanRenderer::VulkanRenderer(size_t width, size_t height, const char* title)
  : m_Window(width, height, title)
  , m_RendererPipeline()
{
}

VulkanRenderer::~VulkanRenderer()
{
  m_RendererPipeline.Shutdown();
}

void
VulkanRenderer::BeginFrame()
{
  if (m_Window.IsActive())
	m_RendererPipeline.BeginFrame();
}

void
VulkanRenderer::Draw(uint32_t vertexCount,
                     uint32_t firstVertex,
                     uint32_t instanceCount,
                     uint32_t firstInstance)
{
  if (m_Window.IsActive())
	m_RendererPipeline.Draw(
	  vertexCount, instanceCount, firstVertex, firstInstance);
}

void
VulkanRenderer::EndFrame()
{
  if (m_Window.IsActive())
	m_RendererPipeline.EndFrame();
}

} // AliusModules
