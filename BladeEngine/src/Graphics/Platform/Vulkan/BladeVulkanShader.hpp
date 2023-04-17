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
  VkShaderModule fragmentModule;

private:
  VkShaderModule CreateShaderModule(std::vector<char> data, VkDevice device);

  void DestroyVertexModule(VkDevice device);
  void DestroyFragmentModule(VkDevice device);
};
} // namespace Vulkan
} // namespace Graphics
} // namespace BladeEngine
