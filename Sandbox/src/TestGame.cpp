#include "TestGame.hpp"

#include "BladeEngine.hpp"
#include "Utils/Random.hpp"

#include <sstream>

namespace BladeEngine
{
    struct Controller
    {
        KeyCode Left, Right;
        KeyCode Jump;

        float Movespeed;
        float JumpForce;
    };

    struct Player {};

    Graphics::Texture2D* g_TextureChickBoy;
    Graphics::Texture2D* g_TexturePlatformBlock;

    std::vector<Graphics::Texture2D*> g_BackgroundTextures;

    // TODO: Move this to a load assets function in engine side
    void LoadTextures()
    {
        g_TextureChickBoy = new Graphics::Texture2D("../assets/sprites/Chick-Boy Free Pack/tile000.png");
        g_TextureChickBoy->CreateGPUTexture();

        g_TexturePlatformBlock = new Graphics::Texture2D(
            "../assets/Sunny-land-assets-files/PNG/environment/props/block-big.png");
        g_TexturePlatformBlock->CreateGPUTexture();

        g_BackgroundTextures.resize(6);
        for (size_t i = 0; i < 6; i++)
        {
            std::stringstream path;
            path << "../assets/sprites/PineForestParallax/MorningLayer" << i + 1 << ".png";

            Graphics::Texture2D* tex = new Graphics::Texture2D(path.str());
            g_BackgroundTextures[i] = tex;
            g_BackgroundTextures[i]->CreateGPUTexture();
        }
    }

    void Move(flecs::entity e, Rigidbody2D& rb, const Controller& ctrl)
    {
        float desiredVel = 0.0f;
        desiredVel += Input::GetKey(ctrl.Left) ? -ctrl.Movespeed : 0.0f;
        desiredVel += Input::GetKey(ctrl.Right) ? ctrl.Movespeed : 0.0f;

        Vec2 currentVel = Physics2D::GetBodyVelocity(rb);
        float velChange = desiredVel - currentVel.X;
        float impulse = Physics2D::GetBodyMass(rb) * velChange;

        Physics2D::AddImpulse(rb, Vec2(impulse, 0.0f));

        if (Input::GetKeyDown(ctrl.Jump))
        {
            Physics2D::AddImpulse(rb, Vec2(0.0f, 1.0f), ctrl.JumpForce);
        }
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
        ground->SetComponent<Rotation>({ 0.0f });
        ground->SetComponent<Scale>({ { 40.0f, 2.0f } });
        ground->AddComponent<Rigidbody2D>();
        ground->AddComponent<BoxCollider2D>();
        ground->GetComponent<BoxCollider2D>()->HalfExtents = { 20.0f, 1.0f };

        ground->SetComponent<Sprite2D>({ g_TexturePlatformBlock });

        {
            auto chickBoy = World::CreateEntity("Chick Boy 1");
            chickBoy->SetComponent<Position>({ { -10.0f, 20.0f } });
            chickBoy->SetComponent<Rotation>({ 0.0f });
            chickBoy->SetComponent<Scale>({ { 1.0f, 1.0f } });
            chickBoy->AddComponent<Rigidbody2D>();
            auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
            chickBoyRB->LockRotation = true;
            chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
            chickBoy->AddComponent<BoxCollider2D>();

            chickBoy->SetComponent<Sprite2D>({ g_TextureChickBoy });
        }
        
        {
            auto chickBoy = World::CreateEntity("Chick Boy 2");
            chickBoy->SetComponent<Position>({ { -5.0f, 20.0f } });
            chickBoy->SetComponent<Rotation>({ 0.0f });
            chickBoy->SetComponent<Scale>({ { 1.0f, 1.0f } });
            chickBoy->AddComponent<Rigidbody2D>();
            auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
            chickBoyRB->LockRotation = true;
            chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
            chickBoy->AddComponent<BoxCollider2D>();

            chickBoy->SetComponent<Sprite2D>({ g_TextureChickBoy });
        }

        {
            auto chickBoy = World::CreateEntity("Chick Boy 3");
            chickBoy->SetComponent<Position>({ { 5.0f, 20.0f } });
            chickBoy->SetComponent<Rotation>({ 0.0f });
            chickBoy->SetComponent<Scale>({ { 1.0f, 1.0f } });
            chickBoy->AddComponent<Rigidbody2D>();
            auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
            chickBoyRB->LockRotation = true;
            chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
            chickBoy->AddComponent<BoxCollider2D>();

            chickBoy->SetComponent<Sprite2D>({ g_TextureChickBoy });
        }

        {
            auto chickBoy = World::CreateEntity("Chick Boy 4");
            chickBoy->SetComponent<Position>({ { 10.0f, 20.0f } });
            chickBoy->SetComponent<Rotation>({ 0.0f });
            chickBoy->SetComponent<Scale>({ { 1.0f, 1.0f } });
            chickBoy->AddComponent<Rigidbody2D>();
            auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
            chickBoyRB->LockRotation = true;
            chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
            chickBoy->AddComponent<BoxCollider2D>();

            chickBoy->SetComponent<Sprite2D>({ g_TextureChickBoy });
        }

        auto player = World::CreateEntity("Player");
        player->SetComponent<Position>({ { 0.0f, 5.0f } });
        player->SetComponent<Rotation>({ 0.0f });
        player->SetComponent<Scale>({ { 1.0f, 1.0f } });
        player->SetComponent<Controller>({ KeyCode::A, KeyCode::D, KeyCode::Space, 4.0f, 5.0f });
        player->SetComponent<Sprite2D>({ g_TextureChickBoy });
        player->AddComponent<Player>();
        player->AddComponent<Rigidbody2D>();
        auto playerRB = player->GetComponent<Rigidbody2D>();
        playerRB->LockRotation = true;
        playerRB->Type = Rigidbody2D::BodyType::Dynamic;
        player->AddComponent<CircleCollider2D>();

        for (size_t i = 0; i < 6; i++)
        {
            std::stringstream ss("Background Layer ");
            ss << i + 1;

            auto background = World::CreateEntity(ss.str());
            background->SetComponent<Position>({ { 0.0f, 0.0f } });
            background->SetComponent<Rotation>({ 0.0f });
            background->SetComponent<Scale>({ { 50.0f, 50.0f } });

            background->SetComponent<Sprite2D>({ g_BackgroundTextures[i] });
        }
        
        World::BindSystem<Rigidbody2D, const Controller>(flecs::OnUpdate, "Move", Move);

        World::BindSystem<const Player, const Position>(flecs::OnUpdate, "Focus Camera", FocusCamera);
    }

    Game* CreateGameInstance()
    {
        return new TestGame();
    }
}