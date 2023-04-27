#include "BladeVulkanDescriptorPool.hpp"

using namespace BladeEngine::Graphics::Vulkan;

BladeEngine::Graphics::Vulkan::VulkanDescriptorPool::VulkanDescriptorPool(
    VkDevice device, std::vector<VkDescriptorType> types,
    std::vector<uint32_t> counts, uint32_t maxSets) {
  std::vector<VkDescriptorPoolSize> poolSizes;

  for (int i = 0; i < types.size(); i++) {
    VkDescriptorPoolSize size{};
    size.type = types[i];
    size.descriptorCount = counts[i];
    poolSizes.push_back(size);
  }

  VkDescriptorPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = maxSets;

  if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

void BladeEngine::Graphics::Vulkan::VulkanDescriptorPool::Dispose(
    VkDevice device) {}

void BladeEngine::Graphics::Vulkan::VulkanDescriptorPool::
    UpdateDescriptorSetImageInfo(VkDevice device, uint32_t descriptorSetIndex,
                                 VkDescriptorImageInfo imageInfo,
                                 uint32_t binding, uint32_t count) {
  VkWriteDescriptorSet descriptorWrite{};
  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = descriptorSets[descriptorSetIndex];
  descriptorWrite.dstBinding = binding;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  descriptorWrite.descriptorCount = count;
  descriptorWrite.pImageInfo = &imageInfo;

  vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void BladeEngine::Graphics::Vulkan::VulkanDescriptorPool::
    UpdateDescriptorSetUniformBuffer(VkDevice device,
                                     uint32_t descriptorSetIndex,
                                     VkDescriptorBufferInfo bufferInfo,
                                     uint32_t binding, uint32_t count) {
  VkWriteDescriptorSet descriptorWrite{};
  descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrite.dstSet = descriptorSets[descriptorSetIndex];
  descriptorWrite.dstBinding = binding;
  descriptorWrite.dstArrayElement = 0;
  descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrite.descriptorCount = count;
  descriptorWrite.pBufferInfo = &bufferInfo;

  vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void BladeEngine::Graphics::Vulkan::VulkanDescriptorPool::
    CreateDescriptorSetLayout(
        VkDevice device, std::vector<VkDescriptorSetLayoutBinding> bindings) {
  VkDescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

void BladeEngine::Graphics::Vulkan::VulkanDescriptorPool::CreateDescriptorSets(
    VkDevice device, uint32_t count) {
  std::vector<VkDescriptorSetLayout> layouts(count, descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount = count;
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets.resize(count);
  if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }
}