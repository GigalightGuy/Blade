#include "Time.hpp"

namespace BladeEngine
{
    float Time::s_CurrentWorldTime;
    float Time::s_DeltaTime;

    std::chrono::time_point<std::chrono::high_resolution_clock> Time::s_Start;

    void Time::Init()
    {
        s_Start = std::chrono::high_resolution_clock::now();
        s_CurrentWorldTime = 0.0f;
    }

    void Time::Update()
    {
        float lastFrameTime = s_CurrentWorldTime;
        s_CurrentWorldTime = 
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - s_Start).count() * 0.001f;

        s_DeltaTime = s_CurrentWorldTime - lastFrameTime;
    }

    
}