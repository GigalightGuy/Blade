#pragma once

#include "VulkanResourceAllocator.hpp"

#include "vulkan/vulkan.hpp"

// TODO(Pedro): Move this into base engine header
#define BITWISE_ENUM(T)									\
	inline T operator|(T a, T b)						\
	{													\
		return (T)((uint32_t)a | (uint32_t)b);			\
	}													\
														\
	inline T& operator|=(T& a, T b)						\
	{													\
		return a = (T)((uint32_t)a | (uint32_t)b);		\
	}													\
														\
	inline T operator&(T a, T b)						\
	{													\
		return (T)((uint32_t)a & (uint32_t)b);			\
	}													\
														\
	inline T& operator&=(T& a, T b)						\
	{													\
		return a = (T)((uint32_t)a & (uint32_t)b);		\
	}

namespace BladeEngine::Graphics {

	
	// TODO(Pedro): Move this to appropriate Buffer base class when implemented
	enum class BufferUsage
	{
		None = 0,

		Vertex = 1 << 0,
		Index = 1 << 1,
		Uniform = 1 << 2,
		Storage = 1 << 3,
		Indirect = 1 << 4,
		TransferSource = 1 << 5,
		TransferDestination = 1 << 6
	};

	BITWISE_ENUM(BufferUsage);

	enum class BufferAllocationUsage
	{
		None = 0,

		HostWrite = 1 << 0,
		HostRead = 1 << 1,
		DeviceLocal = 1 << 2
	};

	BITWISE_ENUM(BufferAllocationUsage);

	struct BufferDescription
	{
		uint64_t Size = 0;
		const void* Data = nullptr;
		BufferUsage Usage = BufferUsage::Vertex;
		BufferAllocationUsage AllocationUsage = BufferAllocationUsage::HostWrite;
		bool KeepMapped = false;
	};

}

namespace BladeEngine::Graphics::Vulkan {


	class VulkanBuffer
	{
	public:
		VulkanBuffer(const BufferDescription& description, VulkanResourceAllocator& allocator);
		~VulkanBuffer();

		VulkanBuffer(const VulkanBuffer&) = delete;
		VulkanBuffer& operator=(const VulkanBuffer&) = delete;
		VulkanBuffer(VulkanBuffer&&) = delete;
		VulkanBuffer& operator=(VulkanBuffer&&) = delete;

		void* Map();
		void Unmap();

		void Resize(uint64_t size);

		VkBuffer GetBuffer() { return m_Buffer; }

		uint64_t GetSize() const { return m_Size; }

	private:
		void CreateBuffer(const BufferDescription& description);

	private:
		VulkanResourceAllocator& m_Allocator;

		BufferUsage m_Usage;
		BufferAllocationUsage m_AllocationUsage;

		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VmaAllocation m_Allocation = VK_NULL_HANDLE;

		bool m_KeepMapped = false;
		uint64_t m_Size = 0;
		void* m_MappedData = nullptr;
	
	};

}
