#pragma once
#include "BladeVulkanUtils.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include "../../Vertex.hpp"

namespace BladeEngine{
namespace Graphics{
namespace Vulkan{
    
    struct VulkanMesh
    {
        std::vector<BladeEngine::Graphics::VertexColorTexture> vertices;
        std::vector<uint16_t> indices;
        
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        VkBuffer elementsBuffer;
        VkDeviceMemory elementsBufferMemory;
        
        VkBuffer uniformBuffer;
        VkDeviceMemory uniformBufferMemory;
        void * uniformBufferMapped;

        VkBuffer uniformBufferExtra;
        VkDeviceMemory uniformBufferMemoryExtra;
        void * uniformBufferMappedExtra;
        
        void UpdateUniformBuffer(MVP data,Extra extra);
        
        void Draw(VkCommandBuffer commandBuffer,VkPipelineLayout &pipelineLayout, VkDescriptorSet &descriptorSet);
        void Dispose(VkDevice device);
    };
    
    VulkanMesh LoadMesh(VkPhysicalDevice physicalDevice,VkDevice device,VkQueue graphicsQueue,VkCommandPool commandPool,std::vector<BladeEngine::Graphics::VertexColorTexture> vertices,std::vector<uint16_t> elements);
}
}
}
