#include "World.hpp"

namespace BladeEngine
{
    flecs::world World::s_FlecsWorld;

    template<typename ... Comps>
    void World::BindSystem(flecs::entity_t phase, const char* name, Func<Comps&...> f)
    {
        s_FlecsWorld.system<Comps>(name).kind(phase).each(f);
    }

    template<typename ... Comps>
    void World::BindSystem(flecs::entity_t phase, const char* name, Func<flecs::entity, Comps&...> f)
    {
        s_FlecsWorld.system<Comps>(name).kind(phase).each(f);
    }

    void World::Step(float deltaTime)
    {
        s_FlecsWorld.progress(deltaTime);
    }


}