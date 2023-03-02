#pragma once

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Vulkan
{
    class vulkanDevice;

    class VulkanCommandPool // TODO: maybe turn this into static class VulkanCommandManager
    {
    public:
        VulkanCommandPool(VulkanDevice* device);
        ~VulkanCommandPool();

    private:
        void CreateCommandPool();
        void CreateCommandBuffers();

        VkCommandBuffer VulkanCommandPool::BeginSingleTimeCommands();

	    void VulkanCommandPool::EndSingleTimeCommands(VkCommandBuffer commandBuffer);

    private:
        VkCommandPool m_CommandPoolHandle;

        std::vector<VkCommandBuffer> m_RenderCommandBuffers;
        
        VulkanDevice* m_Device;
    }
}