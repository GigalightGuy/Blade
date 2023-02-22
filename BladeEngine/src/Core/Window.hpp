#pragma once

#include <stdint.h>
#include <vector>

#if BLADE_VULKAN_API
#include "vulkan/vulkan.hpp"
#endif // BLADE_VULKAN_API

#include "Input.hpp"

struct GLFWwindow;

namespace BladeEngine
{
    class Window
    {
    public:
        Window(uint32_t width, uint32_t height, const std::string& title);
        ~Window();

        inline virtual uint32_t GetWidth() const { return m_WindowData.Width; }
		inline virtual uint32_t GetHeight() const { return m_WindowData.Height; }

        void PollEvents();
        void InputTick();

        std::vector<const char*> GetRequiredExtensions() const;

#if BLADE_VULKAN_API
        VkSurfaceKHR CreateWindowSurface(VkInstance vulkanInstance) const;
#endif // BLADE_VULKAN_API

    private:
        void SetupCallbacks();

        void UpdateKeyStates();

        static void SetKeyState(KeyCode key, KeyState state);
		static void SetMouseButtonState(MouseButton button, KeyState state);

        static void SetCursorPos(float x, float y);

    private:
        GLFWwindow* m_WindowHandle;
        std::string m_Title;

        struct WindowData
        {
            uint32_t Width, Height;
        };

        WindowData m_WindowData;
        
    };
}