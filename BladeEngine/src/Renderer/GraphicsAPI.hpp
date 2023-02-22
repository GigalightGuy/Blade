#pragma once

#if BLADE_VULKAN_API
#include "vulkan/vulkan.hpp"
#endif

namespace BladeEngine
{
    class Window;

#if BLADE_VULKAN_API
    namespace Vulkan
    {
        class VulkanDevice;
    }
#endif

    class GraphicsAPI
    {
    public:
        static void Init(Window* window);
        static void Shutdown();

    private:
#if BLADE_VULKAN_API
        static VkInstance s_Instance;
        static Vulkan::VulkanDevice* s_Device;
        static VkSurfaceKHR s_WindowSurface;
#endif

    };
}