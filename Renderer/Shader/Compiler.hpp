#pragma once

#include "Shader/VulkanShader.hpp"

namespace AliusModules {

class Compiler
{
public:
  static VulkanShaderMeta CreateFromSource(
    const std::string& filepath,
    const vk::ShaderStageFlagBits& shaderStage);
  static VulkanShaderMeta CreateFromBinary(
    const std::string& filepath,
    const vk::ShaderStageFlagBits& shaderStage);

private:
  static std::string ReadFromFile(const std::string& filepath);
  static VulkanShaderMeta CompileString(
    const std::string& filepath,
    const vk::ShaderStageFlagBits& shaderStage);
};

} // AliusModules
