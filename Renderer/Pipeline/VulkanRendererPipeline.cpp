#include "VulkanRendererPipeline.hpp"

namespace AliusModules {

VulkanRendererPipeline::VulkanRendererPipeline()
  : m_Instance(new Instance())
  , m_GraphicsPipeline(new GraphicsPipeline(m_Instance))
  , m_CommandPipeline(
      new CommandPipeline(m_Instance, m_GraphicsPipeline->GetSwapchain()))
{
  m_MaxFrameIndex =
    m_GraphicsPipeline->GetSwapchain()->GetMaxConcurrentFrames();
}

VulkanRendererPipeline::~VulkanRendererPipeline()
{
  // deallocate all heap memory
  delete m_CommandPipeline;
  delete m_GraphicsPipeline;
  delete m_Instance;
}

void VulkanRendererPipeline::BeginFrame()
{
  m_ImageIndex = m_GraphicsPipeline->AcquireImage(m_FrameIndex);
  if (m_ImageIndex == UINT32_MAX) {
	m_FrameIndex = 0;
	m_FrameBegun = false;
	return;
  }

  m_FrameBegun = true;

  m_CommandPipeline->ResetBuffer(m_FrameIndex);
  m_CommandPipeline->BeginBuffer(
    m_FrameIndex,
    m_GraphicsPipeline->GetRenderPass(),
    vk::SubpassContents::eInline,
    m_GraphicsPipeline->GetFramebuffer(m_ImageIndex),
    m_GraphicsPipeline->GetPipeline(),
    vk::PipelineBindPoint::eGraphics);
}

void VulkanRendererPipeline::Draw(
  const std::shared_ptr<Alius::RendererObjectBase>& object)
{
  if (!m_FrameBegun)
	return;

  auto implData =
    std::dynamic_pointer_cast<Alius::VkObjectData>(object->GetImplData());

  if (implData == nullptr)
	return;

  m_CommandPipeline->BindVertexBuffer(m_FrameIndex, implData->VertexBuffer);
  if (object->GetIndicesCount() > 0) {
	m_CommandPipeline->BindIndexBuffer(m_FrameIndex, implData->IndexBuffer);
	m_CommandPipeline->DrawIndexed(m_FrameIndex, object->GetIndicesCount());
	return;
  }

  m_CommandPipeline->Draw(m_FrameIndex, object->GetVerticesCount(), 0);
}

void VulkanRendererPipeline::EndFrame()
{
  if (!m_FrameBegun) {
	m_FrameIndex = 0;
	return;
  }

  auto buffer = m_CommandPipeline->EndBuffer(m_FrameIndex);
  m_GraphicsPipeline->SubmitToComputeQueue(m_FrameIndex, buffer);
  m_GraphicsPipeline->PresentQueue(m_FrameIndex, m_ImageIndex);

  m_FrameIndex = (m_FrameIndex + 1) % m_MaxFrameIndex;

  m_FrameBegun = false;
}

void VulkanRendererPipeline::Shutdown()
{
  m_Instance->GetDevice().waitIdle();

  m_CommandPipeline->Cleanup();
  m_GraphicsPipeline->Cleanup();
  m_Instance->Cleanup();
}

void VulkanRendererPipeline::CreateRendererObj(
  const std::shared_ptr<Alius::RendererObjectBase>& obj)
{
  auto device = m_Instance->GetDevice();
  auto objData = obj->GetData();

  vk::Buffer vertexBuffer;
  vk::DeviceMemory vertexMemory;

  auto [vbuffer, vmemory] =
    CreateVertexBuffer(obj->GetVerticesSize(), objData.GetVerticesPointer());

  vertexBuffer = vbuffer;
  vertexMemory = vmemory;

  vk::Buffer indexBuffer;
  vk::DeviceMemory indexMemory;

  if (obj->GetIndicesSize() > 0) {
	auto [ibuffer, imemory] =
	  CreateIndexBuffer(obj->GetIndicesSize(), objData.GetIndicesPointer());
	indexBuffer = ibuffer;
	indexMemory = imemory;
  }

  obj->SetImplData(Alius::VkObjectData{
    vertexBuffer, indexBuffer, vertexMemory, indexMemory });
}

void VulkanRendererPipeline::DestroyRendererObj(
  const Ref<Alius::VkObjectData>& implData)
{
  auto device = m_Instance->GetDevice();
  device.waitIdle();

  device.destroyBuffer(implData->VertexBuffer);
  device.freeMemory(implData->VertexMemory);
  device.destroyBuffer(implData->IndexBuffer);
  device.freeMemory(implData->IndexMemory);
}

std::pair<vk::Buffer, vk::DeviceMemory>
VulkanRendererPipeline::CreateVertexBuffer(uint32_t dataSize, void* dataPointer)
{
  auto stagingVertexBuffer = m_CommandPipeline->CreateBuffer(
    dataSize, vk::BufferUsageFlagBits::eTransferSrc);
  auto stagingVertexBufferMem = m_CommandPipeline->AllocateDeviceMemory(
    stagingVertexBuffer,
    vk::MemoryPropertyFlagBits::eHostCoherent |
      vk::MemoryPropertyFlagBits::eHostVisible);

  m_CommandPipeline->CopyToDeviceMemory(
    stagingVertexBufferMem, 0, dataPointer, dataSize);

  auto vertexBuffer =
    m_CommandPipeline->CreateBuffer(dataSize,
                                    vk::BufferUsageFlagBits::eTransferDst |
                                      vk::BufferUsageFlagBits::eVertexBuffer);
  auto vertexBufferMem = m_CommandPipeline->AllocateDeviceMemory(
    vertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);

  m_CommandPipeline->CopyBuffers(stagingVertexBuffer, vertexBuffer, dataSize);

  // Free temporary resources as we don't need them anymore
  auto device = m_Instance->GetDevice();

  device.destroyBuffer(stagingVertexBuffer);
  device.freeMemory(stagingVertexBufferMem);

  return { vertexBuffer, vertexBufferMem };
}

std::pair<vk::Buffer, vk::DeviceMemory>
VulkanRendererPipeline::CreateIndexBuffer(uint32_t dataSize, void* dataPointer)
{
  auto stagingIndexBuffer = m_CommandPipeline->CreateBuffer(
    dataSize, vk::BufferUsageFlagBits::eTransferSrc);
  auto stagingIndexBufferMem = m_CommandPipeline->AllocateDeviceMemory(
    stagingIndexBuffer,
    vk::MemoryPropertyFlagBits::eHostCoherent |
      vk::MemoryPropertyFlagBits::eHostVisible);
  m_CommandPipeline->CopyToDeviceMemory(
    stagingIndexBufferMem, 0, dataPointer, dataSize);

  auto indexBuffer =
    m_CommandPipeline->CreateBuffer(dataSize,
                                    vk::BufferUsageFlagBits::eTransferDst |
                                      vk::BufferUsageFlagBits::eIndexBuffer);
  auto indexBufferMem = m_CommandPipeline->AllocateDeviceMemory(
    indexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);

  m_CommandPipeline->CopyBuffers(stagingIndexBuffer, indexBuffer, dataSize);

  // Free temporary resources as we don't need them anymore
  auto device = m_Instance->GetDevice();

  device.destroyBuffer(stagingIndexBuffer);
  device.freeMemory(stagingIndexBufferMem);

  return { indexBuffer, indexBufferMem };
}

} // AliusModules
