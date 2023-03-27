#include "World.hpp"

namespace BladeEngine
{
    flecs::world World::s_FlecsWorld;

    template<typename ... Comps>
    void World::BindSystem(Func<Comps...> f, flecs::entity_t phase, const char* name)
    {
        s_FlecsWorld.system<Comps>(name).kind(phase).each(f);
    }

    void World::Step()
    {
        s_FlecsWorld.progress();
    }


}