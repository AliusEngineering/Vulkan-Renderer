#include "Compiler.hpp"

#include "shaderc/shaderc.hpp"

#include "fstream"

namespace AliusModules {

shaderc_shader_kind
ShaderStageToShadercKind(const vk::ShaderStageFlagBits& stage);

VulkanShaderMeta
Compiler::CreateFromSource(const std::string& filepath,
                           const vk::ShaderStageFlagBits& shaderStage)
{
  return CompileString(filepath, shaderStage);
}

VulkanShaderMeta
Compiler::CreateFromBinary(const std::string& filepath,
                           const vk::ShaderStageFlagBits& shaderStage)
{
  std::ifstream file{ filepath, std::ios::in | std::ios::binary };
  if (!file.is_open()) {
	SQD_WARN("Failed to open file at '{}'", filepath);
	return {};
  }

  std::string buff;
  std::string res;
  while (std::getline(file, buff, '\n')) {
	res.append(buff);
	res.push_back('\n');
  }

  file.close();

  std::vector<uint32_t> binary{ res.begin(), res.end() };

  return { binary, static_cast<uint32_t>(binary.size() * sizeof(uint32_t)) };
}

std::string
Compiler::ReadFromFile(const std::string& filepath)
{
  std::ifstream file{ filepath, std::ios::in | std::ios::binary };
  if (!file.is_open()) {
	SQD_WARN("Failed to open file at '{}'", filepath);
	return {};
  }

  std::string buff;
  std::string res;
  while (std::getline(file, buff, '\n')) {
	res.append(buff);
	res.push_back('\n');
  }

  file.close();

  res.begin();

  return res;
}

VulkanShaderMeta
Compiler::CompileString(const std::string& filepath,
                        const vk::ShaderStageFlagBits& shaderStage)
{
  auto strSource = ReadFromFile(filepath);
  if (strSource.empty()) {
	return {};
  }

  shaderc::Compiler compiler{};

  auto compileRes = compiler.CompileGlslToSpv(
    strSource, ShaderStageToShadercKind(shaderStage), filepath.c_str());
  switch (compileRes.GetCompilationStatus()) {
	case shaderc_compilation_status_success: {
	  std::vector<uint32_t> res{ compileRes.begin(), compileRes.end() };
	  return { res, static_cast<uint32_t>(res.size() * sizeof(uint32_t)) };
	}
	default: {
	  SQD_WARN("Shader compilation failed!\n\tDetails: {}",
	           compileRes.GetErrorMessage());
	  return {};
	}
  }
}

shaderc_shader_kind
ShaderStageToShadercKind(const vk::ShaderStageFlagBits& stage)
{
  switch (stage) {
	case vk::ShaderStageFlagBits::eVertex:
	  return shaderc_vertex_shader;
	case vk::ShaderStageFlagBits::eFragment:
	  return shaderc_fragment_shader;
	default:
	  break;
  }

  SQD_WARN("Unsupported shader stage!");
  return {};
}

} // AliusModules