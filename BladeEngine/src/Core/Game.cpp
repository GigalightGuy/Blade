#include "Game.hpp"

#include "World.hpp"

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

        m_Window = new Window();

        World::Init();
    }

    Game::~Game()
    {
        delete m_Window;

        World::Shutdown();
    }
    
    void Game::Exit()
    {
        s_Instance->m_ShouldExit = true;
    }

    void Game::Run()
    {
        while (!m_ShouldExit)
        {
            m_Window->InputTick();
            //Input::Update();
            
            World::Tick();
            World::Render();

            Update();
        }

        CleanUp();
    }

    void Game::CleanUp()
    {

    }
}