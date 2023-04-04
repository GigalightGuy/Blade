#pragma once

#include <chrono>

namespace BladeEngine
{
    class Time
    {
    public:
        /**
         * @brief World time in seconds.
         * 
         * @return time in seconds since the beginning of the world.
         */
        inline static float CurrentWorldTime() { return s_CurrentWorldTime; }

        /**
         * @brief Elapsed time in seconds since last frame.
         * 
         * @return time in seconds since last frame.
         */
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