#pragma once

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Vulkan
{
    const std::vector<const char*> k_DeviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    class VulkanDevice
    {
    public:
        VulkanDevice(VkInstance instance);
        ~VulkanDevice();

    private:
        void PickGPU();

    private:
        VkDevice m_Device;

        VkPhysicalDevice m_GPU;
        VkInstance m_Instance;

        VkSurfaceKHR m_Surface;
    };
}