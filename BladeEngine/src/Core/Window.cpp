#include "Window.hpp"

#if BLADE_VULKAN_API
#define GLFW_INCLUDE_VULKAN
#endif // BLADE_VULKAN_API
#include "GLFW/glfw3.h"

#include "Game.hpp"

namespace BladeEngine
{
    Window::Window(uint32_t width, uint32_t height, const std::string& title)
    {
        m_WindowData.Width = width;
        m_WindowData.Height = height;

        m_Title = title;

        if (!glfwInit())
        {
            return;
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_WindowHandle = glfwCreateWindow(m_WindowData.Width, m_WindowData.Height, m_Title.c_str(), nullptr, nullptr);

        if (!m_WindowHandle)
        {
            return;
        }

        glfwMakeContextCurrent(m_WindowHandle);

        glfwSetWindowUserPointer(m_WindowHandle, &m_WindowData);

        SetupCallbacks();
    }
    
    Window::~Window()
    {
        glfwDestroyWindow(m_WindowHandle);

        glfwTerminate();
    }

    void Window::PollEvents()
    {
        glfwPollEvents();
    }

    void Window::InputTick()
    {
        UpdateKeyStates();
        PollEvents();
    }

    std::vector<const char*> GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		return extensions;
	}

    void Window::SetupCallbacks()
    {
        glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow* window, int width, int height)
        {
            WindowData* data = (WindowData*)window;

            data->Width = width;
            data->Height = height;
        });

        glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            switch (action)
			{
			    case GLFW_PRESS:
			    	SetKeyState((KeyCode)key, KeyState::Down);
			    	break;
    
			    case GLFW_RELEASE:
			    	SetKeyState((KeyCode)key, KeyState::Up);
			    	break;
    
			    default:
			    	break;
			}
        });

        glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow* window, int button, int action, int mods)
        {
            switch (action)
			{
			    case GLFW_PRESS:
			    	SetMouseButtonState((MouseButton)button, KeyState::Down);
			    	break;
			    case GLFW_RELEASE:
			    	SetMouseButtonState((MouseButton)button, KeyState::Up);
			    	break;
			    default:
			    	break;
			}
        });

        glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow* window, double xpos, double ypos)
        {
            SetCursorPos((float)xpos, (float)ypos);
        });

        glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window)
        {
            Game::Exit();
        });
    }
    
    void Window::UpdateKeyStates()
    {
        for (auto key : Input::s_KeyStatesToUpdate)
		{
			if (Input::s_KeyStates.find(key) != Input::s_KeyStates.end())
			{
				if (Input::s_KeyStates[key] == KeyState::Down)
				{
					Input::s_KeyStates[key] = KeyState::Held;
				}
				else if (Input::s_KeyStates[key] == KeyState::Up)
				{
					Input::s_KeyStates[key] = KeyState::Released;
				}
			}
		}
		Input::s_KeyStatesToUpdate.clear();

		for (auto key : Input::s_MouseButtonStatesToUpdate)
		{
			if (Input::s_MouseButtonStates.find(key) != Input::s_MouseButtonStates.end())
			{
				if (Input::s_MouseButtonStates[key] == KeyState::Down)
				{
					Input::s_MouseButtonStates[key] = KeyState::Held;
				}
				else if (Input::s_MouseButtonStates[key] == KeyState::Up)
				{
					Input::s_MouseButtonStates[key] = KeyState::Released;
				}
			}
		}
		Input::s_MouseButtonStatesToUpdate.clear();
    }
    
    void Window::SetKeyState(KeyCode key, KeyState state)
    {
        Input::s_KeyStates[key] = state;
		Input::s_KeyStatesToUpdate.push_back(key);
    }
    
    void Window::SetMouseButtonState(MouseButton button, KeyState state)
    {
        Input::s_MouseButtonStates[button] = state;
		Input::s_MouseButtonStatesToUpdate.push_back(button);
    }
    
    void Window::SetCursorPos(float x, float y)
    {
        Input::s_CursorX = x;
        Input::s_CursorY = y;
    }

}