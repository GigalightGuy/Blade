#pragma once

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Vulkan
{
    class VulkanDevice;

    class VulkanRenderPass
    {
    public:
        VulkanRenderPass(VulkanDevice* device, VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSamples);
        ~VulkanRenderPass();

        inline VkRenderPass GetRenderPassHandle() const { return m_RenderPassHandle; }

    private:
        void CreateRenderPass(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSamples);

    private:
        VkRenderPass m_RenderPassHandle;

        VulkanDevice* m_Device;

    };
}