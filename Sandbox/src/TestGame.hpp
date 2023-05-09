#pragma once

#include "Core/Game.hpp"

namespace BladeEngine
{
    class TestGame : public Game
    {
    private:
        virtual void LoadGameResources() override;
        virtual void UnloadGameResources() override;

        virtual void SetupWorld() override;
    };
}