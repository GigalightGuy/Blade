#include "Physics2D.hpp"

#include "../ECS/World.hpp"

#include "box2d/box2d.h"

namespace BladeEngine
{
    int Physics2D::s_VelocityIterations = 6;
    int Physics2D::s_PositionIterations = 2;

    b2World* Physics2D::s_PhysicsWorld;

    void Physics2D::Init()
    {
        b2Vec2 gravity(0.0f, -9.8f);
        s_PhysicsWorld = new b2World(gravity);
    }
    
    void Physics2D::Shutdown()
    {
        delete s_PhysicsWorld;
    }


}