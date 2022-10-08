#pragma once

#include "VulkanRendererPch.hpp"

#include "Pipeline/Components/Instance.hpp"
#include "Pipeline/Components/Swapchain.hpp"

#include "Shader/VulkanShader.hpp"

namespace AliusModules {

struct PipelineBakedInState
{
  vk::PipelineDynamicStateCreateInfo DynamicState;
  vk::PipelineVertexInputStateCreateInfo VertexInput;
  vk::PipelineInputAssemblyStateCreateInfo InputAssembly;
  vk::PipelineViewportStateCreateInfo ViewportState;
  vk::PipelineRasterizationStateCreateInfo Rasterization;
  vk::PipelineMultisampleStateCreateInfo Multisample;
  vk::PipelineDepthStencilStateCreateInfo DepthStencil;
  vk::PipelineColorBlendAttachmentState ColorBlendAttachment;
  vk::PipelineColorBlendStateCreateInfo ColorBlend;

  vk::PipelineLayoutCreateInfo Layout;
};

struct RenderPassState
{
  vk::AttachmentDescription AttachmentDescription;
  vk::AttachmentReference AttachmentReference;
  vk::SubpassDependency SubpassDependency;
  vk::SubpassDescription SubpassDescription;
};

class GraphicsPipeline
{
public:
  explicit GraphicsPipeline(Ref<Instance> instance);

  uint32_t AcquireImage(uint32_t index);

  bool SubmitToComputeQueue(uint32_t index,
                            const vk::CommandBuffer& commandBuffer);

  bool PresentQueue(uint32_t frameIndex, uint32_t imageIndex);

  Ref<Swapchain> GetSwapchain() { return m_Swapchain; }

  vk::RenderPass& GetRenderPass() { return m_RenderPass; }

  vk::Framebuffer& GetFramebuffer(uint32_t index){
	TRY_EXCEPT(return m_Framebuffers.at(index))
	  THROW_ANY("Failed to get framebuffer at " + std::to_string(index))
  }

  vk::Pipeline& GetPipeline()
  {
	return m_Pipeline;
  }

  void Cleanup();

private:
  void ConstructPipelineBakedInState();
  void ConstructRenderPassState();

  vk::PipelineLayout CreatePipelineLayout();
  vk::RenderPass CreateRenderPass();
  vk::Pipeline CreatePipeline();

  std::vector<vk::Framebuffer> CreateFramebuffers();
  void RecreateFramebuffers();

  void RecreateSwapchain();

private:
  Ref<Instance> m_Instance;
  Ref<Swapchain> m_Swapchain;

  std::vector<VulkanShader> m_Shaders;

  PipelineBakedInState m_PipelineBakedInState;
  RenderPassState m_RenderPassState;

  vk::PipelineLayout m_Layout;
  vk::RenderPass m_RenderPass;
  vk::Pipeline m_Pipeline;

  std::vector<vk::Framebuffer> m_Framebuffers;

  std::vector<vk::Semaphore> m_RenderSemaphores, m_ImageSemaphores;
  std::vector<vk::Fence> m_RenderFences;

private:
  uint32_t m_CurrentImageIndex = 0;

private:
  vk::Semaphore CreateSemaphore();
  vk::Fence CreateFence();

private:
  inline static std::vector<vk::DynamicState> c_PipelineDynamicStates = {
	vk::DynamicState::eViewport,
	vk::DynamicState::eScissor
  };

  inline static constexpr uint64_t c_SynchronizersTimeout = UINT32_MAX;

  inline static constexpr std::array<vk::VertexInputBindingDescription, 1>
    c_WorldObjectVertexInputBindings{
	  { { 0, Alius::Vertex::GetStride(), vk::VertexInputRate::eVertex } }
    };

  inline static constexpr std::array<vk::VertexInputAttributeDescription, 4>
    c_WorldObjectVertexAttributes{
	  { { 0, 0, vk::Format::eR32G32Sfloat, Alius::Vertex::PositionOffset() },
	    { 1, 0, vk::Format::eR32G32B32A32Sfloat, Alius::Vertex::ColorOffset() },
	    { 2, 0, vk::Format::eR32Uint, Alius::Vertex::TexIndexOffset() },
	    { 3, 0, vk::Format::eR32G32Sfloat, Alius::Vertex::TexCoordOffset() } }
    };
};

} // AliusModules
