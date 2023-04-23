#include "Game.hpp"

#include "Time.hpp"
#include "../ECS/World.hpp"
#include "../Components/Components.hpp"
#include "Log.hpp"
#include "../Graphics/GraphicsManager.hpp"
#include "../Physics/Physics2D.hpp"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_fixture.h"

namespace BladeEngine
{
    Game* Game::s_Instance;

    Game::Game()
    {
        if (s_Instance)
        {
            delete this;

            return;
        }

        s_Instance = this;

        m_Window = new Window(1200, 800, "Blade Game");

        Graphics::GraphicsManager::Instance()->Init(m_Window);
    }

    Game::~Game()
    {
        Graphics::GraphicsManager::Instance()->Shutdown();

        delete m_Window;
    }
    
    void Game::Exit()
    {
        s_Instance->m_ShouldExit = true;
    }

    // TODO(Pedro): Probably not the best for this, should change later
    void PopulatePhysicsWorld(flecs::entity e, const Position& pos, Rigidbody2D& rb)
    {
        b2World* physicsWorld = (b2World*)Physics2D::GetPhysicsWorldHandle();

        b2BodyDef bodyDef;
        bodyDef.type = (b2BodyType)rb.Type;
        bodyDef.position.Set(pos.Value.X, pos.Value.Y);
        bodyDef.fixedRotation = rb.LockRotation;

        rb.RuntimeBody = physicsWorld->CreateBody(&bodyDef);

        if (e.has<BoxCollider2D>())
        {
            BoxCollider2D* collider = e.get_mut<BoxCollider2D>();

            b2PolygonShape shape;
            shape.SetAsBox(collider->HalfExtents.X, collider->HalfExtents.Y);

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.density = collider->Material.Density;
            fixtureDef.friction = collider->Material.Friction;
            fixtureDef.restitution = collider->Material.Restitution;
            fixtureDef.restitutionThreshold = collider->Material.RestitutionThreshold;

            rb.RuntimeBody->CreateFixture(&fixtureDef);
        }
    }

    void PhysicsStep(flecs::entity e, Position& pos, const Rigidbody2D& rb)
    {
        b2World* physicsWorld = (b2World*)Physics2D::GetPhysicsWorldHandle();

        physicsWorld->Step(e.delta_time(), Physics2D::GetVelocityIterations(), Physics2D::GetPositionIterations());

        pos.Value.X = rb.RuntimeBody->GetPosition().x;
        pos.Value.Y = rb.RuntimeBody->GetPosition().y;
    }

    void Game::Run()
    {
        BLD_CORE_DEBUG("Game started Running!!");

        Time::Init();
        Physics2D::Init();

        World::BindSystem<const Position, Rigidbody2D>(flecs::OnStart, "Populate physics world", PopulatePhysicsWorld);
        World::BindSystem<Position, const Rigidbody2D>(flecs::PostUpdate, "Physics Step", PhysicsStep);

        SetupWorld();

        while (!m_ShouldExit)
        {
            m_Window->InputTick();
            
            Time::Update();

            World::Step(Time::DeltaTime());
        }

        CleanUp();
    }

    void Game::CleanUp()
    {
        Physics2D::Shutdown();
    }
}