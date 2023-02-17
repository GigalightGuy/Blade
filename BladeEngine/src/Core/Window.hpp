#pragma once

#include <stdint.h>

#include "Input.hpp"

struct GLFWwindow;

namespace BladeEngine
{
    class Window
    {
    public:
        Window();
        ~Window();

        inline virtual uint32_t GetWidth() const { return m_WindowData.Width; }
		inline virtual uint32_t GetHeight() const { return m_WindowData.Height; }

        void PollEvents();
        void InputTick();

    private:
        void SetupCallbacks();

        void UpdateKeyStates();

        static void SetKeyState(KeyCode key, KeyState state);
		static void SetMouseButtonState(MouseButton button, KeyState state);

        static void SetCursorPos(float x, float y);

    private:
        GLFWwindow* m_WindowHandle;

        struct WindowData
        {
            uint32_t Width, Height;
        };

        WindowData m_WindowData;
    };
}