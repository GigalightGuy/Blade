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

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain();
        ~VulkanSwapchain();

    private:
        VkSwapchainKHR m_SwapchainHandle;
        VkSurfaceKHR m_Surface;

        uint32_t m_Width;
        uint32_t m_Height;
    };
}