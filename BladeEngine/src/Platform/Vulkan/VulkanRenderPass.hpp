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

    private:
        void CreateRenderPass(VkFormat swapchainImageFormat, VkSampleCountFlagBits msaaSamples);

    private:
        VkRenderPass m_RenderPass;

        VulkanDevice* m_Device;

    };
}