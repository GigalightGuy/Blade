#include "Game.hpp"

namespace BladeEngine
{
    Game::Game()
    {

    }

    void Game::Run()
    {
        while (!m_ShouldExit)
        {
            Update();
        }
    }

    void Game::Update()
    {
        
    }
}