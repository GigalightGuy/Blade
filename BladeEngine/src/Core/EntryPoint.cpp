#include <iostream>

#include "Game.hpp"
#include "Log.hpp"

int main ()
{
    BladeEngine::Log::Init();

    BladeEngine::Game* game = BladeEngine::CreateGameInstance();

    game->Run();

    delete game;

    return 0;
}