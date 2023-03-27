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
        inline void AddComponent();

        template<typename T>
        inline void RemoveComponent();

        template<typename T>
        inline T& GetComponent() const;

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