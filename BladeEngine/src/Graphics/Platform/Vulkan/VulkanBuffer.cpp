#include "VulkanBuffer.hpp"

#include "../../../Core/Base.hpp"

#include "VulkanCheck.hpp"

namespace BladeEngine::Graphics::Vulkan {

	VulkanBuffer::VulkanBuffer(const BufferDescription& description, VulkanResourceAllocator& allocator)
		: m_Allocator(allocator), m_Usage(description.Usage), 
		m_AllocationUsage(description.AllocationUsage),
		m_Size(description.Size), m_KeepMapped(description.KeepMapped)
	{
		CreateBuffer(description);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		vmaDestroyBuffer(m_Allocator, m_Buffer, m_Allocation);
	}

	void* VulkanBuffer::Map()
	{
		if (m_KeepMapped)
		{
			return m_MappedData;
		}

		void* data;
		vmaMapMemory(m_Allocator, m_Allocation, &data);
		return data;
	}

	void VulkanBuffer::Unmap()
	{
		if (m_KeepMapped)
		{
			return;
		}

		vmaUnmapMemory(m_Allocator, m_Allocation);
	}

	void VulkanBuffer::Resize(uint64_t size)
	{
		m_Size = size;
		CreateBuffer({ m_Size, nullptr, m_Usage, m_AllocationUsage, m_KeepMapped });
	}

	void VulkanBuffer::CreateBuffer(const BufferDescription& description)
	{
		m_Size = description.Size;

		VmaAllocationCreateFlags allocationFlags = 0;

		if (description.KeepMapped)
		{
			allocationFlags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}

		if (((uint32_t)m_AllocationUsage & (uint32_t)BufferAllocationUsage::DeviceLocal) ==
			(uint32_t)BufferAllocationUsage::DeviceLocal)
		{
			allocationFlags |= VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		}
		else
		{
			if (((uint32_t)m_AllocationUsage & (uint32_t)BufferAllocationUsage::HostWrite) ==
				(uint32_t)BufferAllocationUsage::HostWrite)
			{
				allocationFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			}
			else if (((uint32_t)m_AllocationUsage & (uint32_t)BufferAllocationUsage::HostRead) ==
				(uint32_t)BufferAllocationUsage::HostRead) 
			{
				allocationFlags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
			}
		}

		VkBufferUsageFlags bufferUsage = 0;
		if (((uint32_t)description.Usage & (uint32_t)BufferUsage::Vertex) ==
			(uint32_t)BufferUsage::Vertex)
		{
			bufferUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		else if (((uint32_t)description.Usage & (uint32_t)BufferUsage::Index) ==
			(uint32_t)BufferUsage::Index)
		{
			bufferUsage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		else if (((uint32_t)description.Usage & (uint32_t)BufferUsage::Uniform) ==
			(uint32_t)BufferUsage::Uniform)
		{
			bufferUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		else if (((uint32_t)description.Usage & (uint32_t)BufferUsage::Storage) ==
			(uint32_t)BufferUsage::Storage) 
		{
			bufferUsage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}

		if (((uint32_t)description.Usage & (uint32_t)BufferUsage::TransferSource) ==
			(uint32_t)BufferUsage::TransferSource)
		{
			bufferUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}
		if (((uint32_t)description.Usage & (uint32_t)BufferUsage::TransferDestination) ==
			(uint32_t)BufferUsage::TransferDestination)
		{
			bufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		if (((uint32_t)description.Usage & (uint32_t)BufferUsage::Indirect) ==
			(uint32_t)BufferUsage::Indirect)
		{
			bufferUsage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}

		BLD_CORE_ASSERT(bufferUsage != 0, "Buffer usage cannot be empty");

		VkBufferCreateInfo createBufferInfo{};
		createBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createBufferInfo.pNext = nullptr;
		createBufferInfo.flags = 0;
		createBufferInfo.size = description.Size;
		createBufferInfo.usage = bufferUsage;

		VmaAllocationCreateInfo createAllocationInfo{};
		createAllocationInfo.flags = allocationFlags;
		createAllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;

		VmaAllocation oldAllocation = m_Allocation;
		VkBuffer oldBuffer = m_Buffer;

		VmaAllocationInfo allocationInfo{};

		BLD_VK_CHECK(vmaCreateBuffer(m_Allocator, &createBufferInfo,
			&createAllocationInfo, &m_Buffer,
			&m_Allocation, &allocationInfo),
			"Cannot create buffer");


		if (description.KeepMapped) {
			m_MappedData = allocationInfo.pMappedData;
		}

		if (description.Data) {
			void* data = nullptr;
			vmaMapMemory(m_Allocator, m_Allocation, &data);
			memcpy(data, description.Data, description.Size);
			vmaUnmapMemory(m_Allocator, m_Allocation);
		}

		if (oldAllocation && oldBuffer) {
			vmaDestroyBuffer(m_Allocator, oldBuffer, oldAllocation);
		}
	}

}