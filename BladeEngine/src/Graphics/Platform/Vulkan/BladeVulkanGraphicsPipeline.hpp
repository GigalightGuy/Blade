#pragma once

#include "BladeVulkanShader.hpp"
#include "BladeVulkanSwapchain.hpp"
#include "BladeVulkanMesh.hpp"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#define FRAMES_IN_FLIGHT 3  // TODO: Change this define to separate header that all Graphics code can have access to

namespace BladeEngine {
namespace Graphics {
namespace Vulkan {
    
class VulkanGraphicsPipeline 
{
public:
  VulkanGraphicsPipeline(VkDevice device, VkRenderPass renderPass, VulkanShader* shader);
  ~VulkanGraphicsPipeline();

  void Dispose(VkDevice device);
  
  //Init
  VkRenderPass m_RenderPass;
  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;
  
  //Render Loop
  std::array<VkDescriptorPool, FRAMES_IN_FLIGHT> descriptorPools;
  std::array<std::vector<VkDescriptorSet>, FRAMES_IN_FLIGHT> descriptorSets;

  void CreateDescriptorPools(VkDevice device,uint32_t count);
  void CreateDescriptorSets(VkDevice device,uint32_t count, uint32_t frameIndex);  
  void FreeDescriptorSets(VkDevice device, uint32_t frameIndex);
  void UpdateDescriptorSet(VkDevice device,VkImageView imageView, VkSampler sampler, VulkanBuffer& uniformBuffer, uint32_t frameIndex, int descriptorSetIndex);
  
private:
  
  void CreateDescriptorSetLayout(VkDevice device);
  void CreateGraphicsPipeline(VkDevice device, VulkanShader *shader);
};
} // namespace Vulkan
} // namespace Graphics
} // namespace BladeEngine
