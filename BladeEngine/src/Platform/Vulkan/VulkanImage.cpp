#include "VulkanImage.hpp"

#include "VulkanDevice.hpp"
#include "../../Core/Log.hpp"

namespace BladeEngine::Vulkan
{
    VulkanImage::VulkanImage(
            uint32_t width, uint32_t height, uint32_t mipLevels, 
			VkSampleCountFlagBits numSamples, VkFormat format, 
            VkImageTiling tiling, VkImageUsageFlags usage, 
            VkMemoryPropertyFlags memoryProperties, VkImageAspectFlags aspectFlags,
            VulkanDevice* device)
		: m_Width(width), m_Height(height), m_MipLevels(mipLevels), m_Format(format), m_Tiling(tiling) 
    {
        CreateImage(memoryProperties, numSamples, usage);
		CreateImageView();
    }
    
    VulkanImage::~VulkanImage()
    {
        vkDestroyImageView(m_Device->GetLogicalDevice(), m_ImageView, nullptr);
		vkDestroyImage(m_Device->GetLogicalDevice(), m_Image, nullptr);
		vkFreeMemory(m_Device->GetLogicalDevice(), m_ImageMemory, nullptr);
    }
	
	void VulkanImage::CreateImage(VkMemoryPropertyFlags memoryProperties, VkSampleCountFlagBits numSamples, VkImageUsageFlags usage)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_Width;
		imageInfo.extent.height = m_Height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = m_MipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = m_Format;
		imageInfo.tiling = m_Tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = numSamples;
		imageInfo.flags = 0;   // Optional

		if (vkCreateImage(m_Device->GetLogicalDevice(), &imageInfo, nullptr, &m_Image) != VK_SUCCESS)
		{
			BLD_CORE_ERROR("Failed to create vulkan image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_Device->GetLogicalDevice(), m_Image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		uint32_t memoryType = FindMemoryType(memRequirements.memoryTypeBits, memoryProperties);
		if (memoryType == -1)
		{
			BLD_CORE_ERROR("Failed to find memory type to create image!");
		}
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, memoryProperties);

		if (vkAllocateMemory(m_Device->GetLogicalDevice(), &allocInfo, nullptr, &m_ImageMemory) != VK_SUCCESS)
		{
			BLD_CORE_ERROR("Failed to allocate vulkan image memory!");
		}

		vkBindImageMemory(m_Device->GetLogicalDevice(), m_Image, m_ImageMemory, 0); // Might need to move this to a separate method
	}
    
    void VulkanImage::CreateImageView()
    {
        VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = m_Format;
		viewInfo.subresourceRange.aspectMask = m_AspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = m_MipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_Device->GetLogicalDevice(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS)
		{
            BLD_CORE_ERROR("Failed to create Vulkan image view!");
		}
    }
    
    VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
    {
        VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			BLD_CORE_ERROR("Failed to create Vulkan image view!");
		}

		return imageView;
    }

	uint32_t VulkanImage::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_Device->GetGPU(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		return -1;
	}

	bool TryFindSupportedFormat(
		VkPhysicalDevice physicalDevice, 
		const std::vector<VkFormat>& candidates, 
		VkImageTiling tiling, VkFormatFeatureFlags features, 
		VkFormat* outputFormat)
	{
		for (auto format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				(*outputFormat) = format;
				return true;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				(*outputFormat) = format;
				return true;
			}
		}

		BLD_CORE_ERROR("Failed to find supported image format from the given candidates!");

		return false;
	}
		
	bool TryFindDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* outputFormat)
	{
		return TryFindSupportedFormat(
			physicalDevice,
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
			outputFormat
		);
	}

}