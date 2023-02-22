#include "../../Renderer/GraphicsAPI.hpp"

#include "../../Core/Window.hpp"
#include "../../Core/Log.hpp"
#include "VulkanValidationLayers.hpp"
#include "VulkanDevice.hpp"

namespace BladeEngine
{
	VkInstance GraphicsAPI::s_Instance;
    Vulkan::VulkanDevice* GraphicsAPI::s_Device;
	VkSurfaceKHR GraphicsAPI::s_WindowSurface;

    void GraphicsAPI::Init(Window* window)
    {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Game";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);      // DEPRECATED MACRO!!
		appInfo.pEngineName = "Blade GDK";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);           // DEPRECATED MACRO!!
		appInfo.apiVersion = VK_API_VERSION_1_0;

        
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> extensions = window->GetRequiredExtensions();

#if BLADE_DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();


        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
#if BLADE_DEBUG
        auto validationLayers = Vulkan::Debug::GetValidationLayers();
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		Vulkan::Debug::PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
#else
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &s_Instance) != VK_SUCCESS)
		{
			BLD_CORE_ERROR("Failed to create Vulkan instance!");
		}

#if BLADE_DEBUG
		Vulkan::Debug::SetupDebugMessenger(s_Instance);
#endif

		s_WindowSurface = window->CreateWindowSurface(s_Instance);

		s_Device = new Vulkan::VulkanDevice(s_Instance, s_WindowSurface);

    }

    void GraphicsAPI::Shutdown()
    {
		delete s_Device;

#if BLADE_DEBUG
		Vulkan::Debug::DestroyDebugMessenger(s_Instance);
#endif

		vkDestroySurfaceKHR(s_Instance, s_WindowSurface, nullptr);

		vkDestroyInstance(s_Instance, nullptr);
    }

    
}