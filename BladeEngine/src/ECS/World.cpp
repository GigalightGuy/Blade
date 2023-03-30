#include "World.hpp"

namespace BladeEngine
{
    flecs::world World::s_FlecsWorld;

    void World::Step(float deltaTime)
    {
        s_FlecsWorld.progress(deltaTime);
    }


}