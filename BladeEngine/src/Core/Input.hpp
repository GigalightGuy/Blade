#pragma once

#include "KeyCodes.hpp"

#include <utility>
#include <unordered_map>
#include <vector>

namespace BladeEngine 
{

	class Input
	{
	public:
		static void Init();
		static void Shutdown();

		/**
		 * @brief Check if key is being held.
		 * 
		 * @param key the KeyCode of the key we want to check.
		 * @return true if key is being held.
		 */
		static bool GetKey(KeyCode key);
		/**
		 * @brief Check if mouse button is being held.
		 * 
		 * @param button the MouseButton of the button we want to check.
		 * @return true if button is being held.
		 */
		static bool GetMouseButton(MouseButton button);

		/**
		 * @brief Check if key was pressed this frame.
		 * 
		 * @param key the KeyCode of the key we want to check.
		 * @return true if key was pressed this frame.
		 */
		static bool GetKeyDown(KeyCode key);
		/**
		 * @brief Check if mouse button was pressed this frame.
		 * 
		 * @param key the MouseButton of the button we want to check.
		 * @return true if button was pressed this frame.
		 */
		static bool GetMouseButtonDown(MouseButton button);

		/**
		 * @brief Check if key was released this frame.
		 * 
		 * @param key the KeyCode of the key we want to check.
		 * @return true if key was released this frame.
		 */
		static bool GetKeyUp(KeyCode key);
		/**
		 * @brief Check if mouse button was released this frame.
		 * 
		 * @param key the MouseButton of the button we want to check.
		 * @return true if button was released this frame.
		 */
		static bool GetMouseButtonUp(MouseButton button);

		/**
		 * @brief Get the cursor position.
		 * 
		 * @return a pair of floats with the cursor position's x and y.
		 */
		inline static std::pair<float, float> GetCursorPos() { return { s_CursorX, s_CursorY }; }
		/**
		 * @brief Get the cursor position x.
		 * 
		 * @return cursor position x.
		 */
		inline static float GetCursorX() { return s_CursorX; };
		/**
		 * @brief Get the cursor position y.
		 * 
		 * @return cursor position y.
		 */
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
