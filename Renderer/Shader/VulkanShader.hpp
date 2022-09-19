#pragma once

#include "VulkanRendererPch.hpp"

#include "RendererBase/Shader.hpp"

namespace AliusModules {

struct VulkanShaderMeta
{
  std::vector<uint32_t> Code;
  uint32_t Size;
};

class VulkanShader : public Alius::Shader
{
public:
  explicit VulkanShader(const vk::Device& device,
                        const std::string& filepath,
                        const vk::ShaderStageFlagBits& stage);

  void CreateFromSource(std::string_view filepath) override;
  void CreateFromCompiled(std::string_view filepath) override;

  void Bind() override;
  void Unbind() override;

  bool IsOK() const { return m_IsOK; }

  vk::PipelineShaderStageCreateInfo GetStageCreateInfo() const
  {
	return m_PipelineStageCreateInfo;
  };

  vk::ShaderModule GetModule() const { return m_Module; }

private:
  vk::Device m_Device;

  VulkanShaderMeta m_Meta;

  vk::ShaderModule m_Module;
  vk::ShaderStageFlagBits m_Stage;
  vk::PipelineShaderStageCreateInfo m_PipelineStageCreateInfo;

  bool m_IsOK = false;

private:
  void ConstructFromSource(std::string_view filepath);
};

} // AliusModules
