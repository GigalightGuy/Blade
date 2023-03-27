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

    template<typename T>
    void Entity::AddComponent()
    {
        m_FlecsEntity.add<T>();
    }
    
    template<typename T>
    void Entity::RemoveComponent()
    {
        m_FlecsEntity.remove<T>();
    }
    
    template<typename T>
    inline T& Entity::GetComponent() const
    {
        return m_FlecsEntity.get<T>();
    }

    
}