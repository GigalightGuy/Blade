#include "TestGame.hpp"

#include "BladeEngine.hpp"

namespace BladeEngine
{
    struct Position
    {
        Vec2 Value;
    };

    void TestGame::SetupWorld()
    {
        

        
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}