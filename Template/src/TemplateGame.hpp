#pragma once

#include "Core/Game.hpp"

namespace BladeEngine
{
    class TemplateGame : public Game
    {
    private:
        virtual void SetupWorld() override;
    };
}