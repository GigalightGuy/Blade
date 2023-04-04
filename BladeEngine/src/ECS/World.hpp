#pragma once

#include "flecs.h"

#include "Entity.hpp"

namespace BladeEngine
{
    /**
     * @brief Static class that contains and manages the ECS World and its entities
     * 
     */
    class World
    {
    public:
        /**
         * @brief Creates a new Entity in the World.
         * 
         * @return created Entity. 
         */
        inline static Entity* CreateEntity() { return new Entity(s_FlecsWorld); }

        /**
         * @brief Creates a new Entity in the World with a name.
         * 
         * @param entityName the new Entity name.
         * 
         * @return created Entity.
         */
        inline static Entity* CreateEntity(const std::string& entityName) { return new Entity(entityName, s_FlecsWorld); }

        /**
         * @brief Destroys entity and removes it from World.
         * 
         * @param entity the Entity do be destroyed.
         * 
         */
        inline static void DestroyEntity(Entity* entity) { delete entity; }

        /**
         * @brief Creates and binds a system to the World.
         * 
         * @tparam Comps component types that will be part of the query.
         * @tparam Func type of f
         * 
         * @param phase pipeline phase that defines when in a frame the system runs.
         * @param name name of the system.
         * @param f function to be executed when running the system.
         */
        template<typename ... Comps, typename Func>
        static void BindSystem(flecs::entity_t phase, const char* name, Func f)
        {
            s_FlecsWorld.system<Comps...>(name).kind(phase).each(f);
        }

        /**
         * @brief Creates and binds a system to the World.
         * 
         * @tparam Comps component types that will be part of the query.
         * @tparam Func type of f
         * 
         * @param timer interval between each time the system gets ran.
         * @param name name of the system.
         * @param f function to be executed when running the system.
         */
        template<typename ... Comps, typename Func>
        static void BindSystem(float timer, const char* name, Func f)
        {
            s_FlecsWorld.system<Comps...>(name).interval(timer).each(f);
        }

    private:
        static void Step(float deltaTime);

    private:
        static flecs::world s_FlecsWorld;

        friend class Game;

    };
}