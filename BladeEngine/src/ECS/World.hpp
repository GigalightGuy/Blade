#pragma once

#include "flecs.h"

#include "Entity.hpp"

namespace BladeEngine
{
    class World
    {
    public:
        template<typename ... Comps>
        using Func = void (*)(Comps...);

        inline static Entity* CreateEntity() { return new Entity(s_FlecsWorld); }
        inline static Entity* CreateEntity(const std::string& entityName) { return new Entity(entityName, s_FlecsWorld); }
        inline static void DestroyEntity(Entity* entity) { delete entity; }

        template<typename ... Comps>
        static void BindSystem(flecs::entity_t phase, const char* name, Func<Comps&...> f);

        template<typename ... Comps>
        static void BindSystem(flecs::entity_t phase, const char* name, Func<flecs::entity, Comps&...> f);

    private:
        static void Step(float deltaTime);

    private:
        static flecs::world s_FlecsWorld;

        friend class Game;

    };
}