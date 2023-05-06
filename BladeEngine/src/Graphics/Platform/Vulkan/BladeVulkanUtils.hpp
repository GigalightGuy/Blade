#pragma once
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include "../../Vertex.hpp"

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

namespace BladeEngine
{
    namespace Graphics
    {
        namespace Vulkan
        {
            
            struct VP{    
                alignas(16) glm::mat4 view;
                alignas(16) glm::mat4 proj;
            };

            struct Model{    
                alignas(16) glm::mat4 model;
            };

            void CreateBuffer(
                VkPhysicalDevice physicalDevice,
                VkDevice device, 
                VkDeviceSize size,
                VkBufferUsageFlags usage,
                VkMemoryPropertyFlags properties,
                VkBuffer &buffer,
                VkDeviceMemory &bufferMemory
            );
            void CopyBuffer(
                VkDevice device, 
                VkQueue graphicsQueue, 
                VkCommandPool commandPool,
                VkBuffer srcBuffer, 
                VkBuffer dstBuffer, 
                VkDeviceSize size
            );            
            VkCommandBuffer BeginSingleTimeCommands(
                VkDevice device,
                VkCommandPool commandPool
            );
            void EndSingleTimeCommands(
                VkDevice device,
                VkQueue graphicsQueue,
                VkCommandPool commandPool,
                VkCommandBuffer commandBuffer
            );
            uint32_t FindMemoryType(
                VkPhysicalDevice physicalDevice,
                uint32_t typeFilter,
                VkMemoryPropertyFlags properties
            );
            void TransitionImageLayout(
                VkDevice device,
                VkQueue graphicsQueue,
                VkCommandPool commandPool,
                VkImage image, 
                VkFormat format,
                VkImageLayout oldLayout,
                VkImageLayout newLayout
            );            
            void CopyBufferToImage(
                VkDevice device,
                VkQueue graphicsQueue,
                VkCommandPool commandPool,
                VkBuffer buffer, 
                VkImage image,
                uint32_t width,
                uint32_t height
            );
            void CreateImage(
                VkPhysicalDevice physicalDevice,
                VkDevice device,
                uint32_t width,
                uint32_t height,
                VkFormat format, 
                VkImageTiling tiling,
                VkImageUsageFlags usage,
                VkMemoryPropertyFlags properties,
                VkImage& image,
                VkDeviceMemory& imageMemory
            );
        
            
            void CreateVertexBuffer(
                std::vector<BladeEngine::Graphics::VertexTexture> vertices,
                VkPhysicalDevice physicalDevice,
                VkDevice device,
                VkQueue graphicsQueue, 
                VkCommandPool commandPool,
                VkBuffer& vertexBuffer,
                VkDeviceMemory& vertexBufferMemory
            );
            void CreateElementsBuffer(
                std::vector<uint16_t> elements,
                VkPhysicalDevice physicalDevice,
                VkDevice device,
                VkQueue graphicsQueue, 
                VkCommandPool commandPool,
                VkBuffer& elementsBuffer,
                VkDeviceMemory& elementsBufferMemory
            );
            
            void CreateUniformBuffer(
                VkPhysicalDevice physicalDevice,
                VkDevice device, 
                VkDeviceSize size, 
                VkBuffer& uniformBuffer, 
                VkDeviceMemory& uniformBufferMemory, 
                void *& uniformBufferMapped);





            template<typename T>
            VkVertexInputBindingDescription GetBindingDescription(
                uint32_t shaderIndex);

            template<>
            VkVertexInputBindingDescription GetBindingDescription<VertexColor>(uint32_t shaderIndex);

            template<>
            VkVertexInputBindingDescription GetBindingDescription<VertexTexture>(uint32_t shaderIndex);

            template<>
            VkVertexInputBindingDescription GetBindingDescription<VertexColorTexture>(uint32_t shaderIndex);



            template<typename T>
            std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(
                uint32_t shaderIndex);

            template<>
            std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexColor>(uint32_t shaderIndex);

            template<>
            std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexTexture>(uint32_t shaderIndex);

            template<>
            std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexColorTexture>(uint32_t shaderIndex);
        }
    }
}
