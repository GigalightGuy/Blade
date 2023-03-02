#pragma once

#include <stdint.h>
#include <vector>

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
    class VulkanImage;
    class VulkanRenderPass;

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain();
        ~VulkanSwapchain();

        void RecreateSwapchain();

    private:
        void CreateSwapchain();
        void CleanupSwapchain();

        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        void CreateSwapchainImages();

        void CreateColorResources();
		void CreateDepthResources();
        void CreateFramebuffers();

    private:
        VkSwapchainKHR m_SwapchainHandle;

        std::vector<VkFramebuffer> m_Framebuffers;

        VulkanImage* m_ColorImage;
		VulkanImage* m_DepthImage;

        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;

        VkFormat m_SwapchainImageFormat;
        VkExtent2D m_SwapchainExtent;

        VkSampleCountFlagBits m_MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        VulkanRenderPass* m_RenderPass;

        VulkanDevice* m_Device;
    };
}