#include "VulkanSwapchain.hpp"

namespace BladeEngine::Vulkan
{
    SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapchainSupportDetails supportDetails;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &supportDetails.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount)
		{
			supportDetails.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(
				device, surface, &formatCount, supportDetails.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount)
		{
			supportDetails.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device, surface, &presentModeCount, supportDetails.presentModes.data());
		}

		return supportDetails;
    }
	
	void VulkanSwapchain::CreateSwapchain()
	{
		
	}
	
	void VulkanSwapchain::CleanupSwapchain()
	{
		
	}
	
	void VulkanSwapchain::RecreateSwapchain(uint32_t width, uint32_t height)
	{
		
	}


}