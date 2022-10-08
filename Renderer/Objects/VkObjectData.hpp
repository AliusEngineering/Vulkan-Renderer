#pragma once

#include "vulkan/vulkan.hpp"

namespace Alius {

struct VkObjectData : public ObjectImplData
{
  VkObjectData() = default;
  VkObjectData(vk::Buffer vertexBuffer)
    : VertexBuffer(vertexBuffer)
  {
  }

  VkObjectData(vk::Buffer vertexBuffer, vk::Buffer indexBuffer)
    : VertexBuffer(vertexBuffer)
    , IndexBuffer(indexBuffer)
  {
  }

  VkObjectData(vk::Buffer vertexBuffer,
               vk::Buffer indexBuffer,
               vk::DeviceMemory vertexMemory,
               vk::DeviceMemory indexMemory)
    : VertexBuffer(vertexBuffer)
    , IndexBuffer(indexBuffer)
    , VertexMemory(vertexMemory)
    , IndexMemory(indexMemory)
  {
  }

  vk::Buffer VertexBuffer, IndexBuffer;
  vk::DeviceMemory VertexMemory, IndexMemory;
};

}