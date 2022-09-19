#include "Logger.hpp"

#include "VulkanWindow.hpp"

namespace AliusModules {

VulkanWindow::VulkanWindow(size_t width, size_t height, const char* title)
{
  try {
	vkfw::init();
	m_Window = vkfw::createWindow(width, height, title);
  } catch (...) {
	SQD_ERR("Failed to create GLFW window!");
	throw std::runtime_error("Failed to create GLFW window!");
  }
}

vk::SurfaceKHR
VulkanWindow::CreateSurfaceKHR(const vk::Instance& instance)
{
  try {
	return vkfw::createWindowSurface(instance, m_Window);
  } catch (...) {
	SQD_ERR("Failed to create window surface!");
	throw std::runtime_error("Failed to create window surface!");
  }
}

void
VulkanWindow::OnUpdate()
{
  // Poll glfw events
  vkfw::pollEvents();
}

} // AliusModules