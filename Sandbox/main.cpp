#include "VulkanRenderer.hpp"

#define LOAD_RENDERER(store, ...)                                              \
  try {                                                                        \
	auto creator = Alius::Renderer::s_RendererModules.at("AlsVkRenderer");     \
	store = creator(__VA_ARGS__);                                              \
  } catch (...) {                                                              \
	throw std::runtime_error("Failed to allocate AlsVkRenderer!");             \
  }

constexpr size_t c_WinWidth = 720;
constexpr size_t c_WinHeight = 720;

int main()
{
  SQD::Logger::EnableLogger();

  std::shared_ptr<Alius::Renderer> renderer;
  LOAD_RENDERER(renderer, c_WinWidth, c_WinHeight, "Vulkan Renderer Sandbox")

  auto window = renderer->GetWindow();

  Alius::ObjectData QuadData{
	{ -0.5f, 0.5f }, { -0.5f, -0.5f }, { 0.5f, -0.5f }, { 0.5f, 0.5f }
  };

  Alius::ObjectData TriangleData{ { -1.0f, 1.0f },
	                              { 1.0f, 1.0f },
	                              { 1.0f, -1.0f } };

  TriangleData.SetColor({ 0.0f, 1.0f, 0.0f });

  QuadData.SetIndices({ 0, 1, 2, 0, 2, 3 });

  // Interpolate between colors (RGB palette)
  QuadData.SetVertexColor(0, { 1.0f, 0.0f, 0.0f });
  QuadData.SetVertexColor(1, { 0.0f, 1.0f, 0.0f });
  QuadData.SetVertexColor(2, { 0.0f, 0.0f, 1.0f });
  QuadData.SetVertexColor(3, { 0.0f, 0.0f, 0.0f });

  auto FirstQuad = renderer->CreateObject<Alius::Quad>(QuadData);
  auto BgTriangle = renderer->CreateObject<Alius::Triangle>(TriangleData);

  while (!window->ShouldClose()) {
	renderer->BeginFrame();

	renderer->Draw(BgTriangle);
	renderer->Draw(FirstQuad);

	renderer->EndFrame();

	window->OnUpdate();
  }
}