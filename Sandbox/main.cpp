#include "VulkanRenderer.hpp"

constexpr size_t c_WinWidth = 720;
constexpr size_t c_WinHeight = 720;

int
main()
{
  AliusModules::VulkanRenderer renderer{ c_WinWidth,
	                                     c_WinHeight,
	                                     "Vulkan Renderer Sandbox" };

  auto window = renderer.GetWindow();

  while (!window->ShouldClose()) {
	renderer.BeginFrame();
	renderer.Draw(6, 0, 1, 0);
	renderer.EndFrame();

	window->OnUpdate();
  }
}