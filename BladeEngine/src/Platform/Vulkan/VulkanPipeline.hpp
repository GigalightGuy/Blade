#pragma once

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Vulkan
{
    class VulkanPipeline
    {
    public:
        VulkanPipeline();
        ~VulkanPipeline();

    private:
        void CreateGraphicsPipeline();
    };
}