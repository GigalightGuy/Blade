#include "Game.hpp"

#include "Time.hpp"
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

        Time::Init();
    }

    Game::~Game()
    {
        GraphicsAPI::Shutdown();

        delete m_Window;
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
            
            Time::Update();

            World::Step(Time::DeltaTime());

            Update();
        }

        CleanUp();
    }

    void Game::CleanUp()
    {

    }
}