#include "TestGame.hpp"

#include "BladeEngine.hpp"

namespace BladeEngine
{
    struct Age
    {
        float Value;
    };

    void TestGame::SetupWorld()
    {
        auto bob = World::CreateEntity("Bob");
        auto suzanne = World::CreateEntity("Suzanne");
        auto chloe = World::CreateEntity("Chloe");

        bob->SetComponent<Age>({ 10 });
        suzanne->SetComponent<Age>({ 12 });
        chloe->SetComponent<Age>({ 10 });

        auto printAgeSystem = [](flecs::entity e, const Age& age) { BLD_DEBUG("{}'s Age is {}", e.name(), age.Value); };

        //World::BindSystem<const Age>(printAgeSystem, flecs::OnUpdate, "Print Age");
    }

    void TestGame::Update()
    {
        if (Input::GetKeyDown(KeyCode::A))
        {
            BLD_DEBUG("Key A pressed!");
        }

        if (Input::GetKey(KeyCode::Space))
        {
            BLD_DEBUG("Key Space is held!");
        }

        if (Input::GetKeyUp(KeyCode::W))
        {
            BLD_DEBUG("Key W released!");
        }

        if (Input::GetKey(KeyCode::Q))
        {
            BLD_DEBUG("Cursor pos -> x: {}, y: {}", Input::GetCursorX(), Input::GetCursorY());
        }
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}