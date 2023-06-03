#include "Physics2D.hpp"

#include "../ECS/World.hpp"
#include "../Components/Components.hpp"

#include "box2d/box2d.h"

namespace BladeEngine
{
    class BaseContactListener : public b2ContactListener
    {
        virtual void BeginContact(b2Contact* contact) override
        {

        }

        virtual void EndContact(b2Contact* contact) override
        {

        }
    };


    int Physics2D::s_VelocityIterations = 6;
    int Physics2D::s_PositionIterations = 2;

    b2World* Physics2D::s_PhysicsWorld;

    void Physics2D::SetVelocity2D(Rigidbody2D& rb, Vec2 direction, float strength)
    {
        b2Vec2 velocity(direction.X * strength, direction.Y * strength);
        rb.RuntimeBody->SetLinearVelocity(velocity);
    }

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
    
    bool Physics2D::Raycast(Rigidbody2D& rb, Vec2 origin, Vec2 direction, float maxLength, RaycastHitInfo& hitInfo)
    {
        b2RayCastInput input;
        input.p1 = { origin.X, origin.Y };
        input.p2 = { origin.X + direction.X, origin.Y + direction.Y };
        input.maxFraction = maxLength;

        float closestLength = maxLength + 0.0001f;
        b2Vec2 intersectionNormal = { 0.0f, 0.0f };

        b2Body* body = rb.RuntimeBody;
        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
        {
            b2RayCastOutput output;
            if (f->RayCast(&output, input, 0) && output.fraction < closestLength)
            {
                closestLength = output.fraction;
                intersectionNormal = output.normal;
            }
        }

        if (closestLength > maxLength)
        {
            return false;
        }

        hitInfo.Point = origin + direction * closestLength;
        hitInfo.Normal = { intersectionNormal.x, intersectionNormal.y };

        return true;

    }


}