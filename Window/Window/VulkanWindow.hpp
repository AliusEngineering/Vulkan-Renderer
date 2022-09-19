#pragma once

#include "WindowBase/Window.hpp"

#include "vkfw/vkfw.hpp"

namespace AliusModules {

class VulkanWindow : public Alius::Window
{
public:
  explicit VulkanWindow(size_t width, size_t height, const char* title);

  bool ShouldClose() override { return m_Window.shouldClose(); };
  bool IsActive() override { return m_IsActive; };

  void OnUpdate() override;

  static vkfw::Window GetWindow() { return m_Window; }
  static vk::SurfaceKHR CreateSurfaceKHR(const vk::Instance& instance);

  static const char** GetRequiredInstanceExtensions(uint32_t* count)
  {
	return vkfw::getRequiredInstanceExtensions(count);
  }

private:
  static vkfw::Window m_Window;

  bool m_IsActive = false;
};

} // AliusModules
