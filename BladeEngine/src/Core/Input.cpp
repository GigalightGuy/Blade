#include "Input.hpp"

namespace BladeEngine
{
	std::unordered_map<KeyCode, KeyState> Input::s_KeyStates;
	std::unordered_map<MouseButton, KeyState> Input::s_MouseButtonStates;

	std::vector<KeyCode> Input::s_KeyStatesToUpdate;
	std::vector<MouseButton> Input::s_MouseButtonStatesToUpdate;

	float Input::s_CursorX, Input::s_CursorY;

	void Input::Init()
	{
		
	}

	void Input::Shutdown()
	{

	}

	bool Input::GetKey(KeyCode key)
	{
		if (s_KeyStates.find(key) == s_KeyStates.end()) return false;

		return s_KeyStates[key] == KeyState::Down || s_KeyStates[key] == KeyState::Held;
	}

	bool Input::GetMouseButton(MouseButton button)
	{
		if (s_MouseButtonStates.find(button) == s_MouseButtonStates.end()) return false;

		return s_MouseButtonStates[button] == KeyState::Down || s_MouseButtonStates[button] == KeyState::Held;
	}

	bool Input::GetKeyDown(KeyCode key)
	{
		if (s_KeyStates.find(key) == s_KeyStates.end()) return false;

		return s_KeyStates[key] == KeyState::Down;
	}

	bool Input::GetMouseButtonDown(MouseButton button)
	{
		if (s_MouseButtonStates.find(button) == s_MouseButtonStates.end()) return false;

		return s_MouseButtonStates[button] == KeyState::Down;
	}

	bool Input::GetKeyUp(KeyCode key)
	{
		if (s_KeyStates.find(key) == s_KeyStates.end()) return false;

		return s_KeyStates[key] == KeyState::Up;
	}

	bool Input::GetMouseButtonUp(MouseButton button)
	{
		if (s_MouseButtonStates.find(button) == s_MouseButtonStates.end()) return false;

		return s_MouseButtonStates[button] == KeyState::Up;
	}

}