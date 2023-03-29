#pragma once

#include "Core/Game.hpp"

namespace BladeEngine
{
    class TestGame : public Game
    {
    private:
        virtual void SetupWorld() override;

        virtual void Update() override;
    };
}