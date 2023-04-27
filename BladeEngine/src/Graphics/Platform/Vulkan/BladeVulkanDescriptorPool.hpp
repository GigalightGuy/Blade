#pragma once
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan.h>

namespace BladeEngine {
namespace Graphics {
namespace Vulkan {
class VulkanDescriptorPool {
public:
  VulkanDescriptorPool(VkDevice device, std::vector<VkDescriptorType> types,
                       std::vector<uint32_t> counts, uint32_t maxSets);

  ~VulkanDescriptorPool();
  void Dispose(VkDevice device);

  void UpdateDescriptorSetUniformBuffer(VkDevice device,
                                        uint32_t descriptorSetIndex,
                                        VkDescriptorBufferInfo bufferInfo,
                                        uint32_t binding, uint32_t count);

  void UpdateDescriptorSetImageInfo(VkDevice device,
                                    uint32_t descriptorSetIndex,
                                    VkDescriptorImageInfo imageInfo,
                                    uint32_t binding, uint32_t count);

private:
  void
  CreateDescriptorSetLayout(VkDevice device,
                            std::vector<VkDescriptorSetLayoutBinding> bindings);

  void CreateDescriptorSets(VkDevice device, uint32_t count);

  VkDescriptorPool descriptorPool;
  VkDescriptorSetLayout descriptorSetLayout;
  std::vector<VkDescriptorSet> descriptorSets;
};
} // namespace Vulkan
} // namespace Graphics
} // namespace BladeEngine
