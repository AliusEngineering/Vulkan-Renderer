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

  m_Window.callbacks()->on_window_resize =
    [](vkfw::Window const& window, size_t width, size_t height) {
	  SQD_LOG("Window resize caught: {}x{}", width, height);
    };

  m_Window.callbacks()->on_window_iconify = [this](vkfw::Window const&,
                                                   int iconified) {
	SQD_LOG("Window minimized: {}.", static_cast<bool>(iconified));
	if (iconified)
	  m_IsActive = false;
	else
	  m_IsActive = true;
  };

  m_IsActive = true;
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