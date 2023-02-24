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
        VulkanImage(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, VulkanDevice* device);
        ~VulkanImage();

    private:
        void CreateImageView();

    private:
        VkImage m_Image;
        VkImageView m_ImageView;

        VkFormat m_Format;
        VkImageAspectFlags m_AspectFlags;

        uint32_t m_MipLevels;

        VulkanDevice* m_Device;
    };

    std::vector<VulkanImage*> CreateSwapchainImages(std::vector<VkImage> images); // TODO: Move this to VulkanSwapchain class, it makes no sense being here, Idk why I did this :((
}