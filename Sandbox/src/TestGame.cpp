#include "TestGame.hpp"

#include <iostream>

#include "BladeEngine.hpp"

namespace BladeEngine
{
    void TestGame::Update()
    {
        if (Input::GetKeyDown(KeyCode::A))
        {
            std::cout << "Key A pressed!"  << std::endl;
        }

        if (Input::GetKey(KeyCode::Space))
        {
            std::cout << "Key Space pressed!"  << std::endl;
        }

        if (Input::GetKeyUp(KeyCode::W))
        {
            std::cout << "Key W pressed" << std::endl;
        }

        if (Input::GetKey(KeyCode::Q))
        {
            std::cout << "Cursor pos -> x: " << Input::GetCursorX() << ", y: " << Input::GetCursorY() << std::endl;
        }
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}