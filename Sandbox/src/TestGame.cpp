#include "TestGame.hpp"

#include "BladeEngine.hpp"
#include "Utils/Random.hpp"

namespace BladeEngine
{
    struct Position
    {
        Vec2 Value;
    };

    struct Movement
    {
        Vec2 Value;
    };

    struct ShouldPrint {};

    void ChangeMovement(Movement& mov)
    {
        mov.Value = Normalize(Utils::Random::NextVec2(-1.0, 1.0));
    }

    void Move(flecs::entity e, Position& pos, const Movement& mov)
    {
        pos.Value += e.delta_time() * mov.Value;
    }

    void PrintPosition(flecs::entity e, const Position& pos, ShouldPrint shouldPrint)
    {
        BLD_DEBUG("{} position is {}", e.name(), pos.Value);
    }

    void TestGame::SetupWorld()
    {
        for (int i = 0; i < 100000; i++)
        {
            auto e = World::CreateEntity();

            e->SetComponent<Position>({ Utils::Random::NextVec2(-100.0, 100.0) });
            e->SetComponent<Movement>({ Utils::Random::NextVec2(-1.0, 1.0) });
        }

        auto bob = World::CreateEntity("Bob");
        bob->SetComponent<Position>({ Utils::Random::NextVec2(-100.0, 100.0) });
        bob->SetComponent<Movement>({ Utils::Random::NextVec2(-1.0, 1.0) });
        bob->AddComponent<ShouldPrint>();

        World::BindSystem<Movement>(5.0f, "Change Movement", ChangeMovement);
        World::BindSystem<Position, const Movement>(flecs::OnUpdate, "Move", Move);
        World::BindSystem<const Position, ShouldPrint>(flecs::OnUpdate, "Print Position", PrintPosition);

        BLD_DEBUG_ASSERT(false, "TEHE");
        
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}