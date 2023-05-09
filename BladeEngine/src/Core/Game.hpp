#pragma once

#include "Window.hpp"

extern int main();

namespace BladeEngine 
{
	class World;

	class Game 
	{
	public:
  		Game();
  		~Game();

		/**
		 * @brief Tells application to exit.
		 * 
		 */
  		static void Exit();

	private:
		void LoadResources();
		void UnloadResources();

		void LoadCoreResources();
		void UnloadCoreResources();

		virtual void LoadGameResources() = 0;
		virtual void UnloadGameResources() = 0;

		virtual void SetupWorld() = 0;

  		void Run();
  		void CleanUp();

	private:
  		bool m_ShouldExit = false;

  		Window *m_Window;

  		static Game *s_Instance;

  		friend int ::main();
	};

	extern Game *CreateGameInstance();

}