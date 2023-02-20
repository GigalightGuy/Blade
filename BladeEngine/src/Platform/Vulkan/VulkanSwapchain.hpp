#pragma once

#include "stdint.h"

#include "vulkan/vulkan.hpp"

namespace BladeEngine
{
    class VulkanSwapchain
    {
    public:
        VulkanSwapchain();
        ~VulkanSwapchain();

    private:
        VkSwapchainKHR m_SwapchainHandle;
        VkSurfaceKHR m_Surface;

        uint32_t m_Width;
        uint32_t m_Height;
    };
}