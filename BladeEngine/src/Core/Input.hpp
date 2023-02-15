#pragma once

#include "KeyCodes.hpp"

#include <utility>
#include <unordered_map>

namespace BladeEngine 
{

	class Input
	{
	public:
		static void Init();
		static void Shutdown();

		static bool GetKey(KeyCode key);
		static bool GetMouseButton(MouseButton button);

		static bool GetKeyDown(KeyCode key);
		static bool GetMouseButtonDown(MouseButton button);

		static bool GetKeyUp(KeyCode key);
		static bool GetMouseButtonUp(MouseButton button);

		inline static std::pair<float, float> GetCursorPos() { return { s_CursorX, s_CursorY }; }
		inline static float GetCursorX() { return s_CursorX; };
		inline static float GetCursorY() { return s_CursorY; };

	private:
		static std::unordered_map<KeyCode, KeyState> s_KeyStates;
		static std::unordered_map<MouseButton, KeyState> s_MouseButtonStates;

		static std::vector<KeyCode> s_KeyStatesToUpdate;
		static std::vector<MouseButton> s_MouseButtonStatesToUpdate;

        static float s_CursorX, s_CursorY;

		friend class Window;
	};

}
