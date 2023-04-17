#include "BladeVulkanShader.hpp"
#include <vector>

using namespace BladeEngine::Graphics::Vulkan;

VulkanShader::VulkanShader(VkDevice device, std::vector<char> vertexShaderData,
                           std::vector<char> fragmentShaderData) {
  vertexModule = CreateShaderModule(vertexShaderData, device);
  fragmentModule = CreateShaderModule(fragmentShaderData, device);
}

VulkanShader::~VulkanShader() {}

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
