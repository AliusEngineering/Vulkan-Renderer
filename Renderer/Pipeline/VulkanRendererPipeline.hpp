#pragma once

#include "VulkanRendererPch.hpp"

#include "Pipeline/Components/Instance.hpp"
#include "Pipeline/Components/Swapchain.hpp"

#include "Pipeline/Components/CommandPipeline.hpp"
#include "Pipeline/Components/GraphicsPipeline.hpp"

namespace AliusModules {

class VulkanRendererPipeline
{
public:
  explicit VulkanRendererPipeline();
  ~VulkanRendererPipeline() = default;

  void BeginFrame();

  void Draw(const Ref<Alius::RendererObjectBase>& object);

  void EndFrame();

  void Shutdown();

  void CreateRendererObj(const Ref<Alius::RendererObjectBase>& obj);
  void DestroyRendererObj(const Ref<Alius::VkObjectData>& implData);

  std::pair<vk::Buffer, vk::DeviceMemory> CreateVertexBuffer(uint32_t dataSize,
                                                             void* dataPointer);
  std::pair<vk::Buffer, vk::DeviceMemory> CreateIndexBuffer(uint32_t dataSize,
                                                            void* dataPointer);

private:
  Ref<Instance> m_Instance;
  Ref<GraphicsPipeline> m_GraphicsPipeline;
  Ref<CommandPipeline> m_CommandPipeline;

private:
  uint32_t m_FrameIndex = 0;
  uint32_t m_ImageIndex = 0;
  uint32_t m_MaxFrameIndex = 0;

  bool m_FrameBegun = false;
};

} // AliusModules
