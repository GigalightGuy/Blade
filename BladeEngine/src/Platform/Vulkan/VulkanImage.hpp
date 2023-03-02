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
        VulkanImage(
            uint32_t width, uint32_t height, uint32_t mipLevels, 
			VkSampleCountFlagBits numSamples, VkFormat format, 
            VkImageTiling tiling, VkImageUsageFlags usage, 
            VkMemoryPropertyFlags memoryProperties, VkImageAspectFlags aspectFlags,
            VulkanDevice* device,
            bool isDepthBuffer = false); // TODO: Change mipLevels parameter to a bool to tell vulkan to generate mip levels or not
        ~VulkanImage();

        inline VkImage GetImageHandle() const { return m_Image; }
        inline VkImageView GetImageView() const { return m_ImageView; }

    private:
        void CreateImage(VkMemoryPropertyFlags memoryProperties, VkSampleCountFlagBits numSamples, VkImageUsageFlags usage);
        void CreateImageView();

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);

        bool HasStencilComponent(VkFormat format);

    private:
        VkImage m_Image;
        VkImageView m_ImageView;
        VkDeviceMemory m_ImageMemory;

        uint32_t m_Width, m_Height;

        VkFormat m_Format;
        VkImageTiling m_Tiling;
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