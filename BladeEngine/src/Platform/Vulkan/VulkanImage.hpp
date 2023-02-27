#pragma once

#include <stdint.h>
#include <vector>

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Vulkan
{
    class VulkanDevice;

    class VulkanImage
    {
    public:
        VulkanImage(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VulkanDevice* device);
        ~VulkanImage();

    private:
        void CreateImageView();

    private:
        VkImage m_Image;
        VkImageView m_ImageView;
        VkDeviceMemory m_ImageMemory;

        VkFormat m_Format;
        VkImageAspectFlags m_AspectFlags;

        uint32_t m_MipLevels;

        VulkanDevice* m_Device;
    };

    VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    bool TryFindSupportedFormat(
        VkPhysicalDevice physicalDevice, 
        const std::vector<VkFormat>& candidates, 
        VkImageTiling tiling, VkFormatFeatureFlags features, 
        VkFormat* outputFormat);

    bool TryFindDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* outputFormat);

}