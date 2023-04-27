#include "BladeVulkanShader.hpp"
#include <vector>
#include "../../../../vendor/SPIRV-Cross/spirv_cross.hpp"

using namespace BladeEngine::Graphics::Vulkan;

VulkanShader::VulkanShader(VkDevice device, std::vector<char> vertexShaderData,
                           std::vector<char> fragmentShaderData) {
  vertexModule = CreateShaderModule(vertexShaderData, device);
  fragmentModule = CreateShaderModule(fragmentShaderData, device);

  vertexShaderBindings = GetVertexShaderBindings(vertexShaderData);
  fragmentShaderBindings = GetFragmentShaderBindings(fragmentShaderData);
}

VulkanShader::~VulkanShader() {}

std::vector<VkDescriptorSetLayoutBinding> VulkanShader::GetVertexShaderBindings(std::vector<char> vertexShaderData)
{
  std::vector<uint32_t> spvVert(vertexShaderData.size() / sizeof(uint32_t));
  memcpy(spvVert.data(), vertexShaderData.data(), vertexShaderData.size());

  std::vector<VkDescriptorSetLayoutBinding> bindings;
  spirv_cross::Compiler compilerVert = spirv_cross::Compiler(spvVert);
  spirv_cross::ShaderResources resVert = compilerVert.get_shader_resources();

  for (auto& samp : resVert.sampled_images) 
  {
    auto &type = compilerVert.get_type(samp.type_id);
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
    descriptorSetLayoutBinding.binding = compilerVert.get_decoration(samp.id,spv::Decoration::DecorationBinding);
    descriptorSetLayoutBinding.descriptorCount = type.array[0] != 0 ? type.array[0] : 1;
    descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
    descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings.push_back(descriptorSetLayoutBinding);
  }
    
  for (auto& ub : resVert.uniform_buffers) 
  {        
    auto &type = compilerVert.get_type(ub.type_id);
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
    descriptorSetLayoutBinding.binding = compilerVert.get_decoration(ub.id,spv::Decoration::DecorationBinding);
    descriptorSetLayoutBinding.descriptorCount = type.array[0] != 0 ? type.array[0] : 1;
    descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
    descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings.push_back(descriptorSetLayoutBinding);
  }
  return bindings;
}

std::vector<VkDescriptorSetLayoutBinding> VulkanShader::GetFragmentShaderBindings(std::vector<char> fragmentShaderData)
{
  std::vector<uint32_t> spvFrag(fragmentShaderData.size() / sizeof(uint32_t));
  memcpy(spvFrag.data(), fragmentShaderData.data(), fragmentShaderData.size());

  std::vector<VkDescriptorSetLayoutBinding> bindings;
  spirv_cross::Compiler compilerVert = spirv_cross::Compiler(spvFrag);
  spirv_cross::ShaderResources resFrag = compilerVert.get_shader_resources();

  for (auto& samp : resFrag.sampled_images) 
  {
    auto &type = compilerVert.get_type(samp.type_id);
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
    descriptorSetLayoutBinding.binding = compilerVert.get_decoration(samp.id,spv::Decoration::DecorationBinding);
    descriptorSetLayoutBinding.descriptorCount = type.array[0] != 0 ? type.array[0] : 1;
    descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
    descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(descriptorSetLayoutBinding);
  }
    
  for (auto& ub : resFrag.uniform_buffers) 
  {        
    auto &type = compilerVert.get_type(ub.type_id);
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
    descriptorSetLayoutBinding.binding = compilerVert.get_decoration(ub.id,spv::Decoration::DecorationBinding);
    descriptorSetLayoutBinding.descriptorCount = type.array[0] != 0 ? type.array[0] : 1;
    descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
    descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(descriptorSetLayoutBinding);
  }
  return bindings;
}

VkShaderModule VulkanShader::CreateShaderModule(std::vector<char> data,
                                                VkDevice device) {
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = data.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(data.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}

void VulkanShader::Dispose(VkDevice device) {
  DestroyFragmentModule(device);
  DestroyVertexModule(device);
}

void VulkanShader::DestroyVertexModule(VkDevice device) {
  vkDestroyShaderModule(device, vertexModule, nullptr);
}

void VulkanShader::DestroyFragmentModule(VkDevice device) {
  vkDestroyShaderModule(device, fragmentModule, nullptr);
}
