#include "Game.hpp"

#include "Time.hpp"
#include "../ECS/World.hpp"
#include "../Components/Components.hpp"
#include "Log.hpp"
#include "../Graphics/GraphicsManager.hpp"
#include "../Physics/Physics2D.hpp"
#include "../Graphics/Mesh.hpp"

#include "../Audio/BladeAudio.hpp"

#include "box2d/box2d.h"

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
        AudioManager::Init();
    }

    Game::~Game()
    {
        Graphics::GraphicsManager::Instance()->Shutdown();
        AudioManager::Shutdown();
        delete m_Window;
    }
    
    void Game::Exit()
    {
        s_Instance->m_ShouldExit = true;
    }

    void Game::LoadResources()
    {
        LoadCoreResources();
        LoadGameResources();
    }

    void Game::UnloadResources()
    {
        UnloadCoreResources();
        UnloadGameResources();
    }

    void Game::LoadCoreResources()
    {
        Graphics::Mesh::LoadDefaultMeshes();
    }

    void Game::UnloadCoreResources()
    {
        Graphics::Mesh::UnloadDefaultMeshes();
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

            fixtureDef.filter.categoryBits = collider->CollisionLayers;
            fixtureDef.filter.maskBits = collider->CollisionMask;
            fixtureDef.filter.groupIndex = collider->GroupId;

            fixtureDef.isSensor = collider->IsSensor;

            rb.RuntimeBody->CreateFixture(&fixtureDef);
        }

        if (e.has<CircleCollider2D>())
        {
            CircleCollider2D* collider = e.get_mut<CircleCollider2D>();

            b2CircleShape shape;
            shape.m_radius = collider->Radius;

            b2FixtureDef fixtureDef;
            fixtureDef.shape = &shape;
            fixtureDef.density = collider->Material.Density;
            fixtureDef.friction = collider->Material.Friction;
            fixtureDef.restitution = collider->Material.Restitution;
            fixtureDef.restitutionThreshold = collider->Material.RestitutionThreshold;

            fixtureDef.filter.categoryBits = collider->CollisionLayers;
            fixtureDef.filter.maskBits = collider->CollisionMask;
            fixtureDef.filter.groupIndex = collider->GroupId;

            fixtureDef.isSensor = collider->IsSensor;

            rb.RuntimeBody->CreateFixture(&fixtureDef);
        }
    }

    void PrePhysicsStep(const Position& pos, Rigidbody2D& rb)
    {
        rb.RuntimeBody->SetTransform({ pos.Value.X, pos.Value.Y }, 0.0f);
    }

    void PhysicsStep(flecs::iter it)
    {
        b2World* physicsWorld = (b2World*)Physics2D::GetPhysicsWorldHandle();

        physicsWorld->Step(it.delta_time(), Physics2D::GetVelocityIterations(), Physics2D::GetPositionIterations());
    }

    void PostPhysicsStep(Position& pos, const Rigidbody2D& rb)
    {
        pos.Value.X = rb.RuntimeBody->GetPosition().x;
        pos.Value.Y = rb.RuntimeBody->GetPosition().y;
    }

    void BeginDrawing(flecs::iter it) { Graphics::GraphicsManager::Instance()->BeginDrawing(); }

    void EndDrawing(flecs::iter it) { Graphics::GraphicsManager::Instance()->EndDrawing(); }

    void DrawSprite(const Sprite2D& sprite, const Position& pos, const Rotation& rot, const Scale& scale)
    {
        Graphics::GraphicsManager::Instance()->Draw(
                sprite.Texture, glm::vec3(pos.Value.X, pos.Value.Y, 0.0f), 
                glm::vec3(0.0f, 0.0f, rot.Angle), glm::vec3(scale.Value.X, scale.Value.Y, 1.0f));
    }

    void Game::Run()
    {
        BLD_CORE_DEBUG("Loading resources...");

        LoadResources();

        BLD_CORE_DEBUG("Game started Running!!");

        Time::Init();
        Physics2D::Init();

        // Physics World Setup
        World::BindSystem<const Position, Rigidbody2D>(flecs::OnStart, "Populate physics world", PopulatePhysicsWorld);

        // Physics World Update
        World::BindSystem<const Position, Rigidbody2D>(flecs::PostUpdate, "Pre Physics Step", PrePhysicsStep);
        World::BindSystemNoQuery(flecs::PostUpdate, "Physics Step", PhysicsStep);
        World::BindSystem<Position, const Rigidbody2D>(flecs::PostUpdate, "Post Physics Step", PostPhysicsStep);

        // Render
        World::BindSystemNoQuery(flecs::PreStore, "Start Drawing", BeginDrawing);
        World::BindSystem<const Sprite2D, const Position, const Rotation, const Scale>(flecs::PreStore, "Draw Sprite", DrawSprite);
        World::BindSystemNoQuery(flecs::PreStore, "End Drawing", EndDrawing);

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

        UnloadResources();
    }
}