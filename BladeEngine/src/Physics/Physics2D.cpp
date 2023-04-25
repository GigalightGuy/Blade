#include "Physics2D.hpp"

#include "../ECS/World.hpp"
#include "../Components/Components.hpp"

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
    
    void Physics2D::AddImpulse(Rigidbody2D& rb, Vec2 direction, float strength)
    {
        b2Vec2 impulse(direction.X * strength, direction.Y * strength);
        rb.RuntimeBody->ApplyLinearImpulse(impulse, rb.RuntimeBody->GetWorldCenter(), true);
    }
    
    void Physics2D::AddImpulse(Rigidbody2D& rb, Vec2 impulse)
    {
        rb.RuntimeBody->ApplyLinearImpulse(b2Vec2(impulse.X, impulse.Y), rb.RuntimeBody->GetWorldCenter(), true);
    }
    
    void Physics2D::AddForce(Rigidbody2D& rb, Vec2 direction, float strength)
    {
        b2Vec2 force(direction.X * strength, direction.Y * strength);
        rb.RuntimeBody->ApplyForce(force, rb.RuntimeBody->GetWorldCenter(), true);
    }
    
    void Physics2D::AddForce(Rigidbody2D& rb, Vec2 force)
    {
        rb.RuntimeBody->ApplyForce(b2Vec2(force.X, force.Y), rb.RuntimeBody->GetWorldCenter(), true);
    }
    
    Vec2 Physics2D::GetBodyVelocity(Rigidbody2D& rb)
    {
        b2Vec2 velocity = rb.RuntimeBody->GetLinearVelocity();
        return Vec2(velocity.x, velocity.y);
    }
    
    float Physics2D::GetBodyMass(Rigidbody2D& rb)
    {
        return rb.RuntimeBody->GetMass();
    }


}