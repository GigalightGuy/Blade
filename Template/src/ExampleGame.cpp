#include "ExampleGame.hpp"

#include "BladeEngine.hpp"
#include "Utils/Random.hpp"

#include <sstream>

#include "Components/PlayerComponents.hpp"
#include "Systems/PlayerSystems.hpp"

#include "Components/RaindropComponents.hpp"
#include "Systems/RaindropSystems.hpp"

#include "Components/GroundComponents.hpp"

namespace BladeEngine {

Graphics::Texture2D *rainDropTexture;
Graphics::Texture2D *waterBarTexture;
Graphics::Texture2D *bucketTexture;

void LoadContent()
{
    rainDropTexture = new Graphics::Texture2D("assets/sprites/drop.png");
    rainDropTexture->CreateGPUTexture();

    bucketTexture = new Graphics::Texture2D("assets/sprites/bucket.png");
    bucketTexture->CreateGPUTexture();

    waterBarTexture = new Graphics::Texture2D("assets/sprites/water.png");
    waterBarTexture->CreateGPUTexture();
}

void ExampleGame::SetupWorld() 
{
  LoadContent();

  auto raindrop = World::CreateEntity("Raindrop");
  raindrop->SetComponent<Position>({{0.0f, 10.0f}});
  raindrop->SetComponent<Rotation>({0.0f});
  raindrop->SetComponent<Scale>({{1.0f, 1.0f}});
  raindrop->SetComponent<Sprite2D>({rainDropTexture});
  raindrop->SetComponent<Raindrop>({10,false});
  raindrop->AddComponent<Rigidbody2D>();
  auto raindropRB = raindrop->GetComponent<Rigidbody2D>();
  raindropRB->LockRotation = true;
  raindropRB->Type = Rigidbody2D::BodyType::Dynamic;
  raindrop->AddComponent<CircleCollider2D>();

  auto ground = World::CreateEntity("Ground");
  ground->SetComponent<Position>({{0.0f, -6.0f}});
  ground->SetComponent<Rotation>({0.0f});
  ground->SetComponent<Scale>({{40.0f, 2.0f}});
  ground->AddComponent<Ground>();
  ground->AddComponent<Rigidbody2D>();
  ground->AddComponent<BoxCollider2D>();
  ground->GetComponent<BoxCollider2D>()->HalfExtents = {20.0f, 1.0f};

  auto player = World::CreateEntity("Player");
  player->SetComponent<Position>({{0.0f, -5.0f}});
  player->SetComponent<Rotation>({0.0f});
  player->SetComponent<Scale>({{1.0f, 1.0f}});
  player->SetComponent<Controller>(
      {KeyCode::A, KeyCode::D, 5.0f});
  player->SetComponent<Player>({0});
  player->SetComponent<Sprite2D>({bucketTexture});
  player->AddComponent<Rigidbody2D>();
  auto playerRB = player->GetComponent<Rigidbody2D>();
  playerRB->LockRotation = true;
  playerRB->Type = Rigidbody2D::BodyType::Dynamic;
  player->AddComponent<CircleCollider2D>();

  auto waterBar = World::CreateEntity("WaterBar");
  waterBar->SetComponent<Position>({{0.0f, -8.0f}});
  waterBar->SetComponent<Rotation>({0.0f});
  waterBar->SetComponent<Scale>({{1.0f, 1.0f}});
  waterBar->SetComponent<Sprite2D>({waterBarTexture});
  waterBar->AddComponent<WaterBar>();

  World::BindSystem<const Controller, Rigidbody2D, const Position>(flecs::OnUpdate, "Move", Move);

  World::BindSystem<Raindrop, Position, Rigidbody2D>(flecs::OnUpdate, "RebootOnGroundHit", RebootOnGroundHit);

  World::BindSystem<BladeEngine::Scale , const WaterBar>(flecs::OnUpdate, "UpdateUI", UpdateUI);
}

Game *CreateGameInstance() 
{ 
    return new ExampleGame(); 
}
}
