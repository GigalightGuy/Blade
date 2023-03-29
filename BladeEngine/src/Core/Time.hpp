#pragma once

#include <chrono>

namespace BladeEngine
{
    class Time
    {
    public:
        inline static float CurrentWorldTime() { return s_CurrentWorldTime; }
        inline static float DeltaTime() { return s_DeltaTime; }

    private:
        static void Init();
        static void Update();

    private:
        static float s_CurrentWorldTime;
        static float s_DeltaTime;

        static std::chrono::time_point<std::chrono::high_resolution_clock> s_Start;

        friend class Game;
    };
}