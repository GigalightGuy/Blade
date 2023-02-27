#pragma once

#include <stdint.h>
#include <vector>

#include "vulkan/vulkan.hpp"

#include "VulkanImage.hpp"

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

        void RecreateSwapchain(uint32_t width, uint32_t height);

    private:
        void CreateSwapchain();
        void CleanupSwapchain();

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        void CreateSwapchainImages();

    private:
        VkSwapchainKHR m_SwapchainHandle;

        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;

        VkFormat m_SwapchainImageFormat;
        VkExtent2D m_SwapchainExtent;

        uint32_t m_Width;
        uint32_t m_Height;

        VulkanDevice* m_Device;
    };
}