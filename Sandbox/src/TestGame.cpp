#include "TestGame.hpp"

#include "BladeEngine.hpp"
#include "Utils/Random.hpp"

namespace BladeEngine
{
    struct ShouldPrint {};

    Graphics::Texture2D* textureGray;

    void LoadTextures()
    {
        textureGray = new BladeEngine::Graphics::Texture2D("../assets/sprites/Chick-Boy Free Pack/tile000.png");
        textureGray->CreateGPUTexture();
    }

    void PrintPosition(flecs::entity e, const Position& pos, ShouldPrint shouldPrint)
    {
        BLD_DEBUG("{} position is {}", e.name(), pos.Value);
    }

    void DrawSprite(flecs::iter it, const Sprite2D* sprite, const Position* pos)
    {
        Graphics::GraphicsManager::Instance()->BeginDrawing();

        for (auto i : it)
        {
            Graphics::GraphicsManager::Instance()->Draw(
                sprite[i].Texture, glm::vec3(pos[i].Value.X, pos[i].Value.Y, 0.0f), 
                glm::vec3(0.0f), glm::vec3(3.0f));
        }
        
        Graphics::GraphicsManager::Instance()->EndDrawing();
    }

    void TestGame::SetupWorld()
    {
        LoadTextures();

        auto ground = World::CreateEntity("Ground");
        ground->SetComponent<Position>({ { 0.0f, 0.0f } });
        ground->AddComponent<Rigidbody2D>();
        ground->AddComponent<BoxCollider2D>();
        ground->GetComponent<BoxCollider2D>()->HalfExtents = { 20.0f, 1.0f };

        auto bob = World::CreateEntity("Bob");
        bob->SetComponent<Position>({ { 20.0f, 50.0f } });
        bob->AddComponent<Rigidbody2D>();
        auto bobRB = bob->GetComponent<Rigidbody2D>();
        bobRB->LockRotation = true;
        bobRB->Type = Rigidbody2D::BodyType::Dynamic;
        bob->AddComponent<BoxCollider2D>();
        bob->AddComponent<ShouldPrint>();

        bob->SetComponent<Sprite2D>({ textureGray });

        //World::BindSystem<const Position, ShouldPrint>(flecs::OnUpdate, "Print Position", PrintPosition);

        World::BindSystemIter<const Sprite2D, const Position>(flecs::PostUpdate, "Draw Sprite", DrawSprite);
        
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}