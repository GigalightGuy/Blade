#pragma once

#include "flecs.h"

#include "Entity.hpp"

namespace BladeEngine
{
    class World
    {
    public:
        inline static Entity* CreateEntity() { return new Entity(s_FlecsWorld); }
        inline static Entity* CreateEntity(const std::string& entityName) { return new Entity(entityName, s_FlecsWorld); }
        inline static void DestroyEntity(Entity* entity) { delete entity; }

        template<typename ... Comps, typename Func>
        static void BindSystem(flecs::entity_t phase, const char* name, Func f)
        {
            s_FlecsWorld.system<Comps...>(name).kind(phase).each(f);
        }

    private:
        static void Step(float deltaTime);

    private:
        static flecs::world s_FlecsWorld;

        friend class Game;

    };
}