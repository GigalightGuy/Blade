#include "TestGame.hpp"

#include "BladeEngine.hpp"
#include "Utils/Random.hpp"

namespace BladeEngine
{
    struct ShouldPrint {};

    void PrintPosition(flecs::entity e, const Position& pos, ShouldPrint shouldPrint)
    {
        BLD_DEBUG("{} position is {}", e.name(), pos.Value);
    }

    void TestGame::SetupWorld()
    {
        auto ground = World::CreateEntity("Ground");
        ground->SetComponent<Position>({ { 0.0f, 0.0f } });
        ground->AddComponent<Rigidbody2D>();
        ground->AddComponent<BoxCollider2D>();
        ground->GetComponent<BoxCollider2D>()->HalfExtents = { 20.0f, 1.0f };

        auto bob = World::CreateEntity("Bob");
        bob->SetComponent<Position>({ { 0.0f, 50.0f } });
        bob->AddComponent<Rigidbody2D>();
        auto bobRB = bob->GetComponent<Rigidbody2D>();
        bobRB->LockRotation = true;
        bobRB->Type = Rigidbody2D::BodyType::Dynamic;
        bob->AddComponent<BoxCollider2D>();
        bob->AddComponent<ShouldPrint>();

        World::BindSystem<const Position, ShouldPrint>(flecs::OnUpdate, "Print Position", PrintPosition);
        
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}