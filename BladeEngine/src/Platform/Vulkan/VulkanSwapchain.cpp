#include "VulkanSwapchain.hpp"

#include "../../Core/Log.hpp"
#include "VulkanDevice.hpp"
#include "VulkanQueue.hpp"
#include "VulkanImage.hpp"
#include "VulkanRenderPass.hpp"

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
		CreateSwapchain();
	}
	
	VulkanSwapchain::~VulkanSwapchain()
	{
		CleanupSwapchain();
	}

	void VulkanSwapchain::RecreateSwapchain(uint32_t width, uint32_t height)
	{
		vkDeviceWaitIdle(m_Device->GetLogicalDevice());

		CleanupSwapchain();

		CreateSwapchain();
		
		CreateColorResources();
		CreateDepthResources();
		CreateFramebuffers();
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
		createInfo.surface = windowSurface;

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

		m_SwapchainImageFormat = surfaceFormat.format;
		m_SwapchainExtent = extent;

		CreateSwapchainImages();
	}
	
	void VulkanSwapchain::CleanupSwapchain()
	{
		//vkDestroyImageView(m_Device->GetLogicalDevice(), m_ColorImageView, nullptr);
		//vkDestroyImage(m_Device->GetLogicalDevice(), m_ColorImage, nullptr);
		//vkFreeMemory(m_Device->GetLogicalDevice(), m_ColorImageMemory, nullptr);
		delete m_ColorImage;

		//vkDestroyImageView(m_Device->GetLogicalDevice(), m_DepthImageView, nullptr);
		//vkDestroyImage(m_Device->GetLogicalDevice(), m_DepthImage, nullptr);
		//vkFreeMemory(m_Device->GetLogicalDevice(), m_DepthImageMemory, nullptr);
		delete m_DepthImage;

		for (auto framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(m_Device->GetLogicalDevice(), framebuffer, nullptr);
		}

		for (auto imageView : m_SwapchainImageViews)
		{
			vkDestroyImageView(m_Device->GetLogicalDevice(), imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_Device->GetLogicalDevice(), m_SwapchainHandle, nullptr);
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
			m_Width,
			m_Height
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
	
	void VulkanSwapchain::CreateSwapchainImages()
	{
		VkDevice logicalDevice = m_Device->GetLogicalDevice();
		uint32_t imageCount;

		vkGetSwapchainImagesKHR(logicalDevice, m_SwapchainHandle, &imageCount, nullptr);
		m_SwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice, m_SwapchainHandle, &imageCount, m_SwapchainImages.data());

        m_SwapchainImageViews.resize(m_SwapchainImages.size());

		for (size_t i = 0; i < m_SwapchainImages.size(); i++)
		{
			m_SwapchainImageViews[i] = CreateImageView(logicalDevice, m_SwapchainImages[i], 
				m_SwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
	}
	
	void VulkanSwapchain::CreateColorResources()
	{
		VkFormat colorFormat = m_SwapchainImageFormat;

		CreateImage(m_SwapchainExtent.width, m_SwapchainExtent.height, 1, 
			m_MSAASamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_ColorImage, m_ColorImageMemory);

		m_ColorImageView = CreateImageView(m_ColorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
	
	void VulkanSwapchain::CreateDepthResources()
	{
		VkFormat depthFormat;
		if (!TryFindDepthFormat(m_Device->GetGPU(), &depthFormat))
		{
			BLD_CORE_ERROR("Couldn't find Depth Format to create depth image for swap chain!");
		}

		CreateImage(
			m_SwapchainExtent.width, m_SwapchainExtent.height, 1, m_MSAASamples,
			depthFormat, VK_IMAGE_TILING_OPTIMAL, 
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
			m_DepthImage, m_DepthImageMemory);

		m_DepthImageView = CreateImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

		// Not needed because the depth buffer starts out empty 
		// so we don't care about what happens to the initial data of the depthimage
		TransitionImageLayout(m_DepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, 
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
	}

	void VulkanSwapchain::CreateFramebuffers()
	{
		m_Framebuffers.resize(m_SwapchainImageViews.size());
		for (size_t i = 0; i < m_SwapchainImageViews.size(); i++)
		{
			std::array<VkImageView, 3> attachments =
			{
				m_ColorImage->GetImageView(),
				m_DepthImage->GetImageView(),
				m_SwapchainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = m_RenderPass->GetRenderPassHandle();
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = m_SwapchainExtent.width;
			framebufferCreateInfo.height = m_SwapchainExtent.height;
			framebufferCreateInfo.layers = 1;

			if (vkCreateFramebuffer(m_Device->GetLogicalDevice(), &framebufferCreateInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
			{
				BLD_CORE_ERROR("Failed to create Framebuffer!");
			}
		}
	}


}