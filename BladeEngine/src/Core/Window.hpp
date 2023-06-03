#pragma once

#include <stdint.h>
#include <vector>
#include <string>

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

        /**
         * @brief Get application window width.
         * 
         * @return window width.
         */
        inline uint32_t GetWidth() const { return m_WindowData.Width; }
        /**
         * @brief Get application window height.
         * 
         * @return window height.
         */
		inline uint32_t GetHeight() const { return m_WindowData.Height; }

        bool SwapchainNeedsResize() const { return m_WindowData.SwapchainNeedsResize; }

        void GotResized() { m_WindowData.SwapchainNeedsResize = false; }

        static inline float GetViewportAspectRatio() { return s_ViewportAspectRatio; }

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
            bool SwapchainNeedsResize = false;
        };

        WindowData m_WindowData;

        static float s_ViewportAspectRatio;
        
    };
}