#include "VulkanCommandPool.hpp"

#include "../../Core/Log.hpp"
#include "VulkanDevice.hpp"
#include "VulkanQueue.hpp"

namespace BladeEngine::Vulkan
{
    VulkanCommandPool::VulkanCommandPool(VulkanDevice* device)
    {
        CreateCommandPool();
    }
    
    VulkanCommandPool::~VulkanCommandPool()
    {
        vkDestroyCommandPool(m_Device->GetLogicalDevice(), m_CommandPoolHandle, nullptr);
    }
    
    void VulkanCommandPool::CreateCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_Device->GetGPU());

		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

		if (vkCreateCommandPool(m_Device->GetLogicalDevice(), &commandPoolCreateInfo, nullptr, &m_CommandPoolHandle) != VK_SUCCESS)
		{
            BLD_CORE_ERROR("Failed to create command pool!");
		}
    }
    
    void VulkanCommandPool::CreateCommandBuffers()
    {
        m_RenderCommandBuffers.resize(Renderer::GetMaxFramesInFlight);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = m_CommandPoolHandle;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_RenderCommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device->GetLogicalDevice(), &commandBufferAllocateInfo, m_RenderCommandBuffers.data()) != VK_SUCCESS)
		{
            BLD_CORE_ERROR("Failed to allocate command buffers for rendering!");
		}
    }

    // Might want to create separate command pool for this kind of operations with temporary buffers
	// Creating the command pool with the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag
	VkCommandBuffer VulkanCommandPool::BeginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_Device->GetLogicalDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanCommandPool::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_GraphicsQueue);

		vkFreeCommandBuffers(m_Device, m_CommandPool, 1, &commandBuffer);
	}

    
}