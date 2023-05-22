#pragma once

#include "BladeVulkanDevice.hpp"
#include "BladeVulkanSwapchain.hpp"

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Graphics::Vulkan {

	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VulkanDevice* device, VkFormat colorAttachmentFormat, VkFormat depthAttachementFormat);
		~VulkanRenderPass();

		VkRenderPass GetRenderPass() { return m_RenderPass; }

	private:
		void CreateRenderPass(VkFormat colorAttachmentFormat, VkFormat depthAttachementFormat);

	private:
		VulkanDevice* m_Device;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

	};

}