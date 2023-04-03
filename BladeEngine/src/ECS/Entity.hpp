#pragma once

#include <string>

#include "flecs.h"

namespace BladeEngine
{
    class World;

    class Entity
    {
    public:
        template<typename T>
        inline void AddComponent() { m_FlecsEntity.add<T>(); }

        template<typename T>
        inline void SetComponent(T&& value) { m_FlecsEntity.set<T>(value); }

        template<typename T>
        inline void RemoveComponent() { m_FlecsEntity.remove<T>(); }

        template<typename T>
        inline const T& GetComponent() const { return m_FlecsEntity.get<T>(); }

        template<typename T>
        inline T& GetComponent() { return m_FlecsEntity.get_mut<T>(); }

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