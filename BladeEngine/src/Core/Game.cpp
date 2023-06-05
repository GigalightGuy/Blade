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

#include <glm/gtc/type_ptr.hpp>

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

        m_Window = new Window(1920, 1080, "Blade Game");

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

        if (rb.ContactListener)
        {
            rb.ContactListener->SetEntity(e);
            rb.RuntimeBody->GetUserData().pointer = (uintptr_t)rb.ContactListener;
        }

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

    void TransformStep(flecs::iter& it, 
        const Position* localPos, const Rotation* localRot, const Scale* localScale,
        LocalToWorld* transform, const LocalToWorld* parentTransform, const DepthSorting* depth)
    {
        auto parentMat = parentTransform ? glm::make_mat4(parentTransform->Matrix) : glm::mat4(1);

        for (auto i : it)
        {
            auto mat = glm::scale(
                glm::rotate(
                    glm::translate(glm::mat4(1), glm::vec3(localPos[i].Value.X, localPos[i].Value.Y, depth ? depth->ZPos : 0.0f)),
                    localRot[i].Angle,
                    glm::vec3(0, 0, 1)),
                glm::vec3(localScale[i].Value.X, localScale[i].Value.Y, 1));

            mat = parentMat * mat;

            memcpy(transform[i].Matrix, glm::value_ptr(mat), sizeof(float) * 16);
            
        }
        
    }

    void AnimateSprite(flecs::entity e, SpriteAnimator& animator, SpriteRenderer& sprite)
    {
        if (!animator.CurrentAnimation || animator.CurrentAnimation->Frames.size() < 2) return;
        
        float dt = e.delta_time();

        animator.Time += dt * animator.Speed;

        if (animator.Time > animator.CurrentAnimation->FrameDuration)
        {
            size_t frameIndex = (animator.CurrentFrame + 1) % animator.CurrentAnimation->Frames.size();

            bool flipX = sprite.FlipX, flipY = sprite.FlipY;
            sprite = animator.CurrentAnimation->Frames[frameIndex];
            sprite.FlipX = flipX;
            sprite.FlipY = flipY;
            animator.CurrentFrame = frameIndex;
            animator.Time -= animator.CurrentAnimation->FrameDuration;
        }
        
    }

    void BeginDrawing(flecs::iter it) { Graphics::GraphicsManager::Instance()->BeginDrawing(); }

    void EndDrawing(flecs::iter it) { Graphics::GraphicsManager::Instance()->EndDrawing(); }

    void DrawSprite(const SpriteRenderer& sprite, const LocalToWorld& transform)
    {
        Graphics::GraphicsManager::Instance()->DrawSprite(
            sprite.Texture,
            glm::make_mat4(transform.Matrix),
            glm::vec4(
                sprite.FlipX ? sprite.UVDimensions.X + sprite.UVStartPos.X : sprite.UVStartPos.X,
                sprite.FlipY ? sprite.UVDimensions.Y + sprite.UVStartPos.Y : sprite.UVStartPos.Y,
                sprite.FlipX ? -sprite.UVDimensions.X : sprite.UVDimensions.X, 
                sprite.FlipY ? -sprite.UVDimensions.Y : sprite.UVDimensions.Y
            )
        );
    }

    void DrawString(const TextRenderer& text, const LocalToWorld& transform)
    {
        Graphics::GraphicsManager::Instance()->DrawString(
            text.Text, text.Font, glm::make_mat4(transform.Matrix));
    }

    void Game::Run()
    {
        BLD_CORE_DEBUG("Loading resources...");

        LoadResources();

        BLD_CORE_DEBUG("Game started Running!!");

        Time::Init();
        Physics2D::Init();

        // Physics World Setup
        World::GetECSWorldHandle()->system<const Position, Rigidbody2D>("Populate Physics World")
            .kind(flecs::OnStart)
            .each(PopulatePhysicsWorld);
        //World::BindSystem<const Position, Rigidbody2D>(flecs::OnStart, "Populate physics world", PopulatePhysicsWorld);


        // Physics World Update
        World::GetECSWorldHandle()->system<const Position, Rigidbody2D>("Pre Physics Step")
            .kind(flecs::PostUpdate)
            .each(PrePhysicsStep);
        //World::BindSystem<const Position, Rigidbody2D>(flecs::PostUpdate, "Pre Physics Step", PrePhysicsStep);
        World::BindSystemNoQuery(flecs::PostUpdate, "Physics Step", PhysicsStep);
        World::GetECSWorldHandle()->system<Position, const Rigidbody2D>("Post Physics Step")
            .kind(flecs::PostUpdate)
            .each(PostPhysicsStep);
        //World::BindSystem<Position, const Rigidbody2D>(flecs::PostUpdate, "Post Physics Step", PostPhysicsStep);

        World::GetECSWorldHandle()->system<
            const Position, const Rotation, const Scale,
            LocalToWorld, const LocalToWorld, const DepthSorting>("Transform Step")
            .term_at(5).parent().cascade().optional()
            .term_at(6).optional()
            .kind(flecs::PostUpdate)
            .iter(TransformStep);

        World::BindSystem<SpriteAnimator, SpriteRenderer>(flecs::PostUpdate, "Animate Sprite", AnimateSprite);

        // Render
        World::BindSystemNoQuery(flecs::PreStore, "Start Drawing", BeginDrawing);
        World::BindSystem<const SpriteRenderer, const LocalToWorld>(flecs::PreStore, "Draw Sprite", DrawSprite);
        World::BindSystem<const TextRenderer, const LocalToWorld>(flecs::PreStore, "Draw Text", DrawString);
        World::BindSystemNoQuery(flecs::PreStore, "End Drawing", EndDrawing);

        SetupWorld();

        while (!m_ShouldExit)
        {
            if (m_Window->SwapchainNeedsResize())
            {
                Graphics::GraphicsManager::Instance()->RecreateSwapchain(m_Window->GetWidth(), m_Window->GetHeight());

                m_Window->GotResized();
            }

            m_Window->InputTick();
            
            Time::Update();

            World::Step(Time::DeltaTime());
        }

        Graphics::GraphicsManager::Instance()->WaitDeviceIdle();

        CleanUp();
    }

    void Game::CleanUp()
    {
        Physics2D::Shutdown();

        UnloadResources();
    }
}