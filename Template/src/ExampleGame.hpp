#pragma once

#include "Core/Game.hpp"

namespace BladeEngine
{
    class ExampleGame : public Game
    {
    private:
        virtual void SetupWorld() override;
    };
}