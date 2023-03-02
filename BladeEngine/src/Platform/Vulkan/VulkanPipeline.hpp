#pragma once

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Vulkan
{
    class VulkanDevice;
    class VulkanRenderPass;

    class VulkanPipeline
    {
    public:
        VulkanPipeline();
        ~VulkanPipeline();

    private:
        void CreateGraphicsPipeline();

    private:
        VkPipeline m_GraphicsPipeline;

        VkPipelineLayout m_PipelineLayout;

        VulkanRenderPass* m_RenderPass;

        VulkanDevice* m_Device;
    };
}