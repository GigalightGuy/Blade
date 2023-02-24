#include "VulkanSwapchain.hpp"

#include "../../Core/Log.hpp"
#include "VulkanDevice.hpp"
#include "VulkanQueue.hpp"

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
	
	VulkanSwapchain::VulkanSwapchain()
	{
		
	}
	
	VulkanSwapchain::~VulkanSwapchain()
	{
		
	}

	void VulkanSwapchain::RecreateSwapchain(uint32_t width, uint32_t height)
	{
		vkDeviceWaitIdle(m_Device->GetLogicalDevice());

		CleanupSwapchain();

		CreateSwapchain();
		CreateImageViews();
		//CreateColorResources();
		//CreateDepthResources();
		//CreateFramebuffers();
	}
	
	void VulkanSwapchain::CreateSwapchain()
	{
		VkDevice logicalDevice = m_Device->GetLogicalDevice();
		VkPhysicalDevice physicalDevice = m_Device->GetGPU();
		VkSurfaceKHR windowSurface = m_Device->GetWindowSurface();

		SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(physicalDevice, windowSurface);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapchainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapchainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(swapchainSupport.capabilities);

		uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

		if (swapchainSupport.capabilities.maxImageCount > 0 
			&& imageCount > swapchainSupport.capabilities.maxImageCount)
		{
			imageCount = swapchainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, windowSurface);
		uint32_t queueFamilyIndices[] = 
		{ 
			indices.GraphicsFamily.value(), 
			indices.PresentFamily.value() 
		};

		// If queue families differ, we use concurrent mode 
		// to facilitate sharing images from the swap chain 
		// between the different queue families
		if (indices.GraphicsFamily != indices.PresentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else // If we only have one queue family we use exclusive mode to gain performance
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;         // OPTIONAL
			createInfo.pQueueFamilyIndices = nullptr;     // OPTIONAL
		}

		createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &m_SwapchainHandle) != VK_SUCCESS)
		{
			BLD_CORE_ERROR("Failed to create Vulkan Swap Chain!");
		}

		std::vector<VkImage> images;
		vkGetSwapchainImagesKHR(logicalDevice, m_SwapchainHandle, &imageCount, nullptr);
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice, m_SwapchainHandle, &imageCount, images.data());

		m_SwapchainImageFormat = surfaceFormat.format;
		m_SwapchainExtent = extent;
	}
	
	void VulkanSwapchain::CleanupSwapchain()
	{
		
	}
	
	VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
				&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		return availableFormats[0];
	}
	
	VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}
	
	VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		

		VkExtent2D actualExtent =
		{
			static_cast<uint32_t>(m_Width),
			static_cast<uint32_t>(m_Height)
		};

		actualExtent.width = std::clamp(
			actualExtent.width, 
			capabilities.minImageExtent.width, 
			capabilities.maxImageExtent.width
		);
		actualExtent.height = std::clamp(
			actualExtent.height,
			capabilities.minImageExtent.height,
			capabilities.maxImageExtent.height
		);

		return actualExtent;
	}


}