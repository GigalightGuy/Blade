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
            VulkanDevice* device,
			bool isDepthBuffer = false)
		: m_Width(width), m_Height(height), m_MipLevels(mipLevels), m_Format(format), m_Tiling(tiling) 
    {
        CreateImage(memoryProperties, numSamples, usage);
		CreateImageView();

		if (isDepthBuffer)
		{
			TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
		}
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
	
	void VulkanImage::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		//VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_Image;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (HasStencilComponent(m_Format))
			{
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}


		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = m_MipLevels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else
		{
			BLD_CORE_ERROR("Unsupported layout transition!");
		}

		//vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		//EndSingleTimeCommands(commandBuffer);
	}
	
	bool VulkanImage::HasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
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