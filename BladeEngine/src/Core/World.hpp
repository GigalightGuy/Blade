#pragma once

namespace BladeEngine
{
    class World
    {
    private:
        static void Init();
        static void Shutdown();

        static void Tick();
        static void Render();

        friend class Game;
    };
    
}