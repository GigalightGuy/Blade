#pragma once
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

namespace BladeEngine {
namespace Graphics {
namespace Vulkan {
class VulkanShader {
public:
  VulkanShader(VkDevice device, std::vector<char> vertexShaderData,
               std::vector<char> fragmentShaderData);
  ~VulkanShader();

  void Dispose(VkDevice device);
  VkShaderModule vertexModule;
  std::vector<VkDescriptorSetLayoutBinding> vertexShaderBindings;
  VkShaderModule fragmentModule;
  std::vector<VkDescriptorSetLayoutBinding> fragmentShaderBindings;

private:
  VkShaderModule CreateShaderModule(std::vector<char> data, VkDevice device);
  std::vector<VkDescriptorSetLayoutBinding> GetVertexShaderBindings(std::vector<char> vertexShaderData);
  std::vector<VkDescriptorSetLayoutBinding> GetFragmentShaderBindings(std::vector<char> fragmentShaderData);

  void DestroyVertexModule(VkDevice device);
  void DestroyFragmentModule(VkDevice device);
};
} // namespace Vulkan
} // namespace Graphics
} // namespace BladeEngine
