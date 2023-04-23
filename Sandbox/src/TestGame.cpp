#include "TestGame.hpp"

#include "BladeEngine.hpp"
#include "Utils/Random.hpp"

namespace BladeEngine
{
    struct Controller
    {
        KeyCode Left, Right;

        float Movespeed;
    };

    struct Player {};

    Graphics::Texture2D* textureChickBoy;

    // TODO: Move this to a load assets function in engine side
    void LoadTextures()
    {
        textureChickBoy = new BladeEngine::Graphics::Texture2D("../assets/sprites/Chick-Boy Free Pack/tile000.png");
        textureChickBoy->CreateGPUTexture();
    }

    void Move(flecs::entity e, Position& pos, const Controller& ctrl)
    {
        float movement = (Input::GetKey(ctrl.Left) ? -ctrl.Movespeed : 0.0f + 
            Input::GetKey(ctrl.Right) ? ctrl.Movespeed : 0.0f) * e.delta_time();

        pos.Value.X += movement;
    }

    void FocusCamera(const Player& player, const Position& pos)
    {
        Camera::GetMainCamera()->SetPosition({ pos.Value.X, pos.Value.Y, 10.0f });
    }

    void TestGame::SetupWorld()
    {
        LoadTextures();

        auto ground = World::CreateEntity("Ground");
        ground->SetComponent<Position>({ { 0.0f, 0.0f } });
        ground->AddComponent<Rigidbody2D>();
        ground->AddComponent<BoxCollider2D>();
        ground->GetComponent<BoxCollider2D>()->HalfExtents = { 20.0f, 1.0f };

        {
            auto chickBoy = World::CreateEntity("Chick Boy 1");
            chickBoy->SetComponent<Position>({ { -10.0f, 50.0f } });
            chickBoy->AddComponent<Rigidbody2D>();
            auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
            chickBoyRB->LockRotation = true;
            chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
            chickBoy->AddComponent<BoxCollider2D>();

            chickBoy->SetComponent<Sprite2D>({ textureChickBoy });
        }
        
        {
            auto chickBoy = World::CreateEntity("Chick Boy 2");
            chickBoy->SetComponent<Position>({ { -5.0f, 50.0f } });
            chickBoy->AddComponent<Rigidbody2D>();
            auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
            chickBoyRB->LockRotation = true;
            chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
            chickBoy->AddComponent<BoxCollider2D>();

            chickBoy->SetComponent<Sprite2D>({ textureChickBoy });
        }

        {
            auto chickBoy = World::CreateEntity("Chick Boy 3");
            chickBoy->SetComponent<Position>({ { 0.0f, 50.0f } });
            chickBoy->AddComponent<Rigidbody2D>();
            auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
            chickBoyRB->LockRotation = true;
            chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
            chickBoy->AddComponent<BoxCollider2D>();

            chickBoy->SetComponent<Sprite2D>({ textureChickBoy });
        }

        {
            auto chickBoy = World::CreateEntity("Chick Boy 4");
            chickBoy->SetComponent<Position>({ { 5.0f, 50.0f } });
            chickBoy->AddComponent<Rigidbody2D>();
            auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
            chickBoyRB->LockRotation = true;
            chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
            chickBoy->AddComponent<BoxCollider2D>();

            chickBoy->SetComponent<Sprite2D>({ textureChickBoy });
        }

        {
            auto chickBoy = World::CreateEntity("Chick Boy 5");
            chickBoy->SetComponent<Position>({ { 10.0f, 50.0f } });
            chickBoy->AddComponent<Rigidbody2D>();
            auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
            chickBoyRB->LockRotation = true;
            chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
            chickBoy->AddComponent<BoxCollider2D>();

            chickBoy->SetComponent<Sprite2D>({ textureChickBoy });
        }

        auto player = World::CreateEntity("Player");
        player->SetComponent<Position>({ { 0.0f, 0.0f } });
        player->SetComponent<Controller>({ KeyCode::A, KeyCode::D, 2.0f });
        player->SetComponent<Sprite2D>({ textureChickBoy });
        player->AddComponent<Player>();
        
        World::BindSystem<Position, const Controller>(flecs::OnUpdate, "Move", Move);

        World::BindSystem<const Player, const Position>(flecs::OnUpdate, "Focus Camera", FocusCamera);
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}