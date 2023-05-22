#include "BladeVulkanSwapchain.hpp"

#include "BladeVulkanDevice.hpp"
#include "BladeVulkanQueueFamily.hpp"
#include "BladeVulkanUtils.hpp"
#include "VulkanCheck.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace BladeEngine::Graphics::Vulkan {

	void VulkanSwapchain::CreateFramebuffers(VkDevice device, VkRenderPass renderPass)
	{
		std::vector<VkFramebuffer> framebuffers(imageViews.size());

		for (size_t i = 0; i < imageViews.size(); i++) 
		{
			std::array<VkImageView, 2> attachments = { imageViews[i], depthImageView };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = extent.width;
			framebufferInfo.height = extent.height;
			framebufferInfo.layers = 1;

			BLD_VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]),
				"Failed to create framebuffer");
		}

		m_FramebuffersMap[renderPass] = framebuffers;
	}


	VkFormat VulkanSwapchain::FindSupportedFormat(
		VkPhysicalDevice physicalDevice, const std::vector<VkFormat>& candidates,
		VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR &&
				(props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
				(props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

	VkFormat VulkanSwapchain::FindDepthFormat(VkPhysicalDevice physicalDevice) {
		return FindSupportedFormat(
			physicalDevice,
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
			 VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	void VulkanSwapchain::CreateDepthResources(VkPhysicalDevice physicalDevice,
		VkDevice device) {
		VkFormat depthFormat = FindDepthFormat(physicalDevice);

		CreateImage(
			physicalDevice, device, extent.width, extent.height, depthFormat,
			VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);

		depthImageView = CreateImageView(device, depthImage, depthFormat,
			VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(
		const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapchain::ChooseSwapExtent(
		Window* window, const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width !=
			std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {

			VkExtent2D actualExtent = { window->GetWidth(), window->GetHeight() };

			actualExtent.width =
				std::clamp(actualExtent.width, capabilities.minImageExtent.width,
					capabilities.maxImageExtent.width);
			actualExtent.height =
				std::clamp(actualExtent.height, capabilities.minImageExtent.height,
					capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	VulkanSwapchain::VulkanSwapchain(BladeEngine::Window* window,
		VkPhysicalDevice physicalDevice,
		VkDevice device, VkSurfaceKHR surface) {
		CreateSwapchain(window, physicalDevice, device, surface);
		CreateImageViews(device);
		CreateDepthResources(physicalDevice, device);
	}

	void VulkanSwapchain::CreateSwapchain(BladeEngine::Window* window,
		VkPhysicalDevice physicalDevice,
		VkDevice device, VkSurfaceKHR surface) {
		SwapChainSupportDetails swapChainSupport =
			QuerySwapChainSupport(physicalDevice, surface);

		VkSurfaceFormatKHR surfaceFormat =
			ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode =
			ChooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = ChooseSwapExtent(window,
			swapChainSupport.capabilities);

		this->imageFormat = surfaceFormat.format;
		this->extent = extent;

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
			imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		GraphicsFamily graphicsFamily = GetGraphicsFamily(physicalDevice, surface);

		if (graphicsFamily.graphicsFlagIndex.value() !=
			graphicsFamily.presentFlagIndex.value()) {
			uint32_t indices[] = {
				graphicsFamily.graphicsFlagIndex.value(),
				graphicsFamily.presentFlagIndex.value(),
			};
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = indices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());
	}

	VkImageView VulkanSwapchain::CreateImageView(VkDevice device, VkImage image,
		VkFormat format,
		VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	void VulkanSwapchain::CreateImageViews(VkDevice device) {

		imageViews.resize(images.size());

		for (uint32_t i = 0; i < images.size(); i++) {
			imageViews[i] = CreateImageView(device, images[i], imageFormat,
				VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void BladeEngine::Graphics::Vulkan::VulkanSwapchain::Dispose(VkDevice device)
	{
		vkDestroyImage(device, depthImage, nullptr);
		vkDestroyImageView(device, depthImageView, nullptr);
		vkFreeMemory(device, depthImageMemory, nullptr);

		for (auto image : images) {
			vkDestroyImage(device, image, nullptr);
		}

		for (auto imageView : imageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}
		vkDestroySwapchainKHR(device, swapchain, nullptr);
	}

}
