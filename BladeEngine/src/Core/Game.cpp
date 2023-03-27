#include "Game.hpp"

#include "../ECS/World.hpp"
#include "Log.hpp"
#include "../Renderer/GraphicsAPI.hpp"

namespace BladeEngine
{
    Game* Game::s_Instance;

    Game::Game()
    {
        if (s_Instance)
        {
            delete this;

            return;
        }

        s_Instance = this;

        m_Window = new Window(900, 600, "Blade Game");

        GraphicsAPI::Init(m_Window);

        //World::Init();
    }

    Game::~Game()
    {
        GraphicsAPI::Shutdown();

        delete m_Window;

        //World::Shutdown();
    }
    
    void Game::Exit()
    {
        s_Instance->m_ShouldExit = true;
    }

    void Game::Run()
    {
        BLD_CORE_DEBUG("Game started Running!!");

        while (!m_ShouldExit)
        {
            m_Window->InputTick();
            //Input::Update();
            
            //World::Tick();
            //World::Render();

            Update();
        }

        CleanUp();
    }

    void Game::CleanUp()
    {

    }
}