#include "EmptyGame.hpp"

#include "BladeEngine.hpp"

#include <sstream>

namespace BladeEngine {

void ExampleGame::LoadGameResources()
{
}

void ExampleGame::UnloadGameResources()
{
}

void ExampleGame::SetupWorld()
{
}

Game *CreateGameInstance() 
{ 
    return new ExampleGame(); 
}
}
