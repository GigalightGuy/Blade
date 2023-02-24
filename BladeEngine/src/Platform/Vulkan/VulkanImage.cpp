#include "VulkanImage.hpp"

#include "VulkanDevice.hpp"
#include "../../Core/Log.hpp"

namespace BladeEngine::Vulkan
{
    VulkanImage::VulkanImage(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VulkanDevice* device)
    {
        
    }
    
    VulkanImage::VulkanImage(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VulkanDevice* device)
    {
        m_Image = image;
        m_Format = format;
        m_AspectFlags = aspectFlags;
        m_MipLevels = mipLevels;
        m_Device = device;

        CreateImageView();

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

    std::vector<VulkanImage*> CreateSwapchainImages(std::vector<VkImage> images)
    {
        std::vector<VulkanImage*> outputImages;
        outputImages.resize(images.size());

        for (size_t i = 0; i < images.size(); i++)
        {
            outputImages[i] = new VulkanImage(images[i]);
        }

        return outputImages;
    }


}