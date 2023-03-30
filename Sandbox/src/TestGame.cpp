#include "TestGame.hpp"

#include "BladeEngine.hpp"

namespace BladeEngine
{
    struct Age
    {
        int Value;
    };

    void IncreaseAge(Age& age)
    {
        if (Input::GetKeyDown(KeyCode::Enter))
        {
            age.Value++;
        }
    }

    void TestGame::SetupWorld()
    {
        auto bob = World::CreateEntity("Bob");
        auto suzanne = World::CreateEntity("Suzanne");
        auto chloe = World::CreateEntity("Chloe");

        bob->SetComponent<Age>({ 10 });
        suzanne->SetComponent<Age>({ 12 });
        chloe->SetComponent<Age>({ 11 });

        auto printAge = [](flecs::entity e, const Age& age) { BLD_DEBUG("{}'s Age is {}", e.name(), age.Value); };

        World::BindSystem<Age>(flecs::OnUpdate, "Print Age", printAge);
        World::BindSystem<Age>(flecs::OnUpdate, "Increase Age", IncreaseAge);
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}