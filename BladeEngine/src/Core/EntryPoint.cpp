#pragma once

#include <iostream>

#include "Game.hpp"

int main ()
{
    BladeEngine::Game* game = BladeEngine::CreateGameInstance();

    game->Run();

    delete game;

    return 0;
}