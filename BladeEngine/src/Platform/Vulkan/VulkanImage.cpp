#include "VulkanImage.hpp"

#include "VulkanDevice.hpp"
#include "../../Core/Log.hpp"

namespace BladeEngine::Vulkan
{
    VulkanImage::VulkanImage(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VulkanDevice* device)
    {
        
    }
    
    VulkanImage::~VulkanImage()
    {
        
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