#include "Window.hpp"

#include "GLFW/glfw3.h"

#include "Game.hpp"

namespace BladeEngine
{
    Window::Window()
    {
        if (!glfwInit())
        {
            return;
        }

        m_WindowHandle = glfwCreateWindow(900, 600, "BladeGDK Sandbox", nullptr, nullptr);

        if (!m_WindowHandle)
        {
            return;
        }

        glfwMakeContextCurrent(m_WindowHandle);

        SetupCallbacks();
    }
    
    Window::~Window()
    {
        glfwDestroyWindow(m_WindowHandle);

        glfwTerminate();
    }

    void Window::SetupCallbacks()
    {
        glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {

        });

        glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow* window, int button, int action, int mods)
        {

        });

        glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow* window, double xpos, double ypos)
        {

        });

        glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window)
        {
            Game::Exit();
        });
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }
    
    void Window::Update()
    {
        
    }


}