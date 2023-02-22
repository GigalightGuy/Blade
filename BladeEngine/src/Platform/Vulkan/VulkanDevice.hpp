#pragma once

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Vulkan
{
    const std::vector<const char*> k_DeviceExtensions =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    class VulkanQueue;

    class VulkanDevice
    {
    public:
        VulkanDevice(VkInstance instance, VkSurfaceKHR windowSurface);
        ~VulkanDevice();

        inline const VkDevice GetLogicalDevice() const { return m_Device; }

    private:
        void PickGPU();
        bool IsDeviceSuitable(VkPhysicalDevice device);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        void CreateLogicalDevice();

    private:
        VkDevice m_Device;

        VkPhysicalDevice m_GPU;
        VkInstance m_Instance;

        VkSurfaceKHR m_Surface;

        VulkanQueue* m_GraphicsQueue;
        VulkanQueue* m_PresentQueue;
    };
}