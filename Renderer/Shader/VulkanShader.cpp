#include "VulkanShader.hpp"

#include <utility>

#include "Shader/Compiler.hpp"

namespace AliusModules {

VulkanShader::VulkanShader(const vk::Device& device,
                           const std::string& filepath,
                           const vk::ShaderStageFlagBits& stage)
  : m_Device(device)
  , m_Stage(stage)
{
  ConstructFromSource(filepath);
}

void
VulkanShader::CreateFromSource(std::string_view filepath)
{
  m_Meta = Compiler::CreateFromSource(filepath.data(), m_Stage);

  if (m_Meta.Code.empty()) {
	SQD_WARN("Failed to construct VulkanShader!");
	m_IsOK = false;
	return;
  }

  vk::ShaderModuleCreateInfo createInfo{ {}, m_Meta.Size, m_Meta.Code.data() };
  TRY_EXCEPT(m_Module = m_Device.createShaderModule(createInfo); m_IsOK = true;
             m_PipelineStageCreateInfo = { {}, m_Stage, m_Module })
  WARN_ANY("Failed to create shader module!")
}

void
VulkanShader::ConstructFromSource(std::string_view filepath)
{
  CreateFromSource(filepath);
}

void
VulkanShader::CreateFromCompiled(std::string_view filepath)
{
  m_Meta = Compiler::CreateFromBinary(filepath.data(), m_Stage);

  if (m_Meta.Code.empty()) {
	SQD_WARN("Failed to construct VulkanShader!");
	m_IsOK = false;
	return;
  }

  vk::ShaderModuleCreateInfo createInfo{ {}, m_Meta.Size, m_Meta.Code.data() };
  TRY_EXCEPT(m_Module = m_Device.createShaderModule(createInfo); m_IsOK = true;
             m_PipelineStageCreateInfo = { {}, m_Stage, m_Module })
  WARN_ANY("Failed to create shader module!")
}

void
VulkanShader::Bind()
{
}

void
VulkanShader::Unbind()
{
}

} // AliusModules