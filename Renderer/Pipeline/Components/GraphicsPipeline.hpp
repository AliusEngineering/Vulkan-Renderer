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
  explicit GraphicsPipeline(Instance* instance);

  /**
   * @param index Index of image to acquire from swapchain.
   * @return Index of acquired image or UINT32_MAX on failure. Use later to get
   * the proper framebuffer.
   */
  uint32_t AcquireImage(uint32_t index);

  /**
   * @param index Index of current frame
   * @param commandBuffer Command buffer reference to submit.
   * @return True if the result of submission is not error.
   */
  bool SubmitToComputeQueue(uint32_t index,
                            const vk::CommandBuffer& commandBuffer);

  /**
   *
   * @param frameIndex Index of current frame (to get appropriate synchronizers,
   * etc.)
   * @param imageIndex Index of image(-s) to present.
   * @return True if presentation resulted in no errors.
   */
  bool PresentQueue(uint32_t frameIndex, uint32_t imageIndex);

  Swapchain* GetSwapchain() { return m_Swapchain; }

  vk::RenderPass GetRenderPass() const { return m_RenderPass; }
  vk::Framebuffer GetFramebuffer(uint32_t index) const {
	TRY_EXCEPT(return m_Framebuffers.at(index))
	  THROW_ANY("Failed to get framebuffer at " + std::to_string(index))

  }

  vk::Pipeline GetPipeline() const
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
  void DestroyFramebuffers();
  void RecreateFramebuffers();

  void RecreateSwapchain();

private:
  Instance* m_Instance;
  Swapchain* m_Swapchain;

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
  uint32_t m_CurrentImageIndex;

private:
  vk::Semaphore CreateSemaphore();
  vk::Fence CreateFence();

private:
  inline static std::vector<vk::DynamicState> c_PipelineDynamicStates = {
	vk::DynamicState::eViewport,
	vk::DynamicState::eScissor
  };

  inline static constexpr uint64_t c_SynchronizersTimeout = UINT32_MAX;
};

} // AliusModules
