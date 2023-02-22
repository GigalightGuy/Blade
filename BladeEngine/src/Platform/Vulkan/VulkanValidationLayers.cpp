#include "VulkanValidationLayers.hpp"

#include "../../Core/Log.hpp"

namespace BladeEngine::Vulkan::Debug
{
    static const std::vector<const char*> s_ValidationLayers =
    {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> GetValidationLayers()
    {
        return s_ValidationLayers;
    }

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            BLD_CORE_ERROR("Vulkan: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            BLD_CORE_WARN("Vulkan: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            BLD_CORE_INFO("Vulkan: {}", pCallbackData->pMessage);
        }
        else
        {
            BLD_CORE_TRACE("Vulkan: {}", pCallbackData->pMessage);
        }

		return VK_FALSE;
	}

    VkResult CreateDebugUtilsMessengerEXT(
	    VkInstance instance,
	    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	    const VkAllocationCallbacks* pAllocator,
	    VkDebugUtilsMessengerEXT* pMessenger)
    {
	    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	    if (func)
	    {
		    return func(instance, pCreateInfo, pAllocator, pMessenger);
	    }
	    else
	    {
	    	return VK_ERROR_EXTENSION_NOT_PRESENT;
	    }

    }

    void DestroyDebugUtilsMessengerEXT(
    	VkInstance instance,
    	VkDebugUtilsMessengerEXT messenger,
    	const VkAllocationCallbacks* pAllocator)
    {
    	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    	if (func)
    		func(instance, messenger, pAllocator);
    }

    static VkDebugUtilsMessengerEXT s_DebugMessenger;

    void SetupDebugMessenger(VkInstance instance)
	{
		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		PopulateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &s_DebugMessenger) != VK_SUCCESS)
		{
			BLD_CORE_ERROR("Failed to set up Vulkan Debug Messenger!");
		}
	}

	void DestroyDebugMessenger(VkInstance instance)
	{
		DestroyDebugUtilsMessengerEXT(instance, s_DebugMessenger, nullptr);
	}
}