#include "Entity.hpp"

namespace BladeEngine
{
    Entity::Entity(const flecs::world& world)
    {
        m_FlecsEntity = world.entity();
    }
    
    Entity::Entity(const std::string& name, const flecs::world& world)
    {
        m_FlecsEntity = world.entity(name.c_str());
    }
    
    Entity::~Entity()
    {
        m_FlecsEntity.destruct();
    }

    
}