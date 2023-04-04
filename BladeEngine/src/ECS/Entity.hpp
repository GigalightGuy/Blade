#pragma once

#include <string>

#include "flecs.h"

namespace BladeEngine
{
    class World;

    class Entity
    {
    public:
        /**
         * @brief Creates and adds a new component of type T to this Entity.
         * 
         * @tparam T component type.
         */
        template<typename T>
        inline void AddComponent() { m_FlecsEntity.add<T>(); }

        /**
         * @brief Sets the data for the component of type T in this Entity.
         * Creates and adds component if entity doesn't have component of type T.
         * 
         * @tparam T component type.
         * 
         * @param value component value.
         */
        template<typename T>
        inline void SetComponent(T&& value) { m_FlecsEntity.set<T>(value); }

        /**
         * @brief Removes component of type T from this Entity if it exists.
         * 
         * @tparam T component type.
         */
        template<typename T>
        inline void RemoveComponent() { m_FlecsEntity.remove<T>(); }

        /**
         * @brief Get component of type T from this Entity if it exists.
         * 
         * @tparam T component type.
         * 
         * @return constant reference to the component.
         */
        template<typename T>
        inline const T& GetComponent() const { return m_FlecsEntity.get<T>(); }

        /**
         * @brief Get component of type T from this Entity if it exists.
         * 
         * @tparam T component type.
         * 
         * @return reference to the component.
         */
        template<typename T>
        inline T& GetComponent() { return m_FlecsEntity.get_mut<T>(); }

        /**
         * @brief Get this Entity's name.
         * 
         * @return name.
         */
        inline const char* GetName() const { return m_FlecsEntity.name(); }

    private:
        Entity(const flecs::world& world);
        Entity(const std::string& name, const flecs::world& world);
        ~Entity();

    private:
        flecs::entity m_FlecsEntity;

        friend World;
    };
}