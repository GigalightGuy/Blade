#include "TestGame.hpp"

#include <iostream>

namespace BladeEngine
{
    void TestGame::Update()
    {
        std::cout << "OnUpdate" << std::endl;
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}