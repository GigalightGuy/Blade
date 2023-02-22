#pragma once

#include "stdint.h"

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Vulkan
{
    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
	    std::vector<VkSurfaceFormatKHR> formats;
	    std::vector<VkPresentModeKHR> presentModes;
    };

    SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

    class VulkanDevice;

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain();
        ~VulkanSwapchain();

        void CreateSwapchain();
        void CleanupSwapchain();
        void RecreateSwapchain(uint32_t width, uint32_t height);

    private:
        VkSwapchainKHR m_SwapchainHandle;
        VkSurfaceKHR m_Surface;

        uint32_t m_Width;
        uint32_t m_Height;

        VulkanDevice* m_Device;
    };
}