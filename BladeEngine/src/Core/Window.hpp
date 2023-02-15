#pragma once

struct GLFWwindow;

namespace BladeEngine
{
    class Window
    {
    public:
        Window();
        ~Window();

        void PollEvents();
        void Update();

    private:
        void SetupCallbacks();

    private:
        GLFWwindow* m_WindowHandle;
    };
}