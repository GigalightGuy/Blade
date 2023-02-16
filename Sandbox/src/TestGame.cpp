#include "TestGame.hpp"

#include <iostream>

#include "BladeEngine.hpp"

namespace BladeEngine
{
    void TestGame::Update()
    {
        if (Input::GetKeyDown(KeyCode::A))
        {
            BLD_DEBUG("Key A pressed!");
        }

        if (Input::GetKey(KeyCode::Space))
        {
            BLD_DEBUG("Key Space is held!");
        }

        if (Input::GetKeyUp(KeyCode::W))
        {
            BLD_DEBUG("Key W released!");
        }

        if (Input::GetKey(KeyCode::Q))
        {
            BLD_DEBUG("Cursor pos -> x: {}, y: {}", Input::GetCursorX(), Input::GetCursorY());
        }
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}