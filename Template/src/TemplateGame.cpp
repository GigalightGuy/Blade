#include "TemplateGame.hpp"

#include "BladeEngine.hpp"
#include "Utils/Random.hpp"

#include <sstream>

#include "Components/PlayerComponents.hpp"
#include "Systems/PlayerSystems.hpp"

namespace BladeEngine {

Graphics::Texture2D *rainDropTexture;
Graphics::Texture2D *bucketTexture;

void LoadContent()
{
    rainDropTexture = new Graphics::Texture2D("assets/sprites/drop.png");
    rainDropTexture->CreateGPUTexture();
    bucketTexture = new Graphics::Texture2D("assets/sprites/bucket.png");
    bucketTexture->CreateGPUTexture();
}

void TemplateGame::SetupWorld() 
{
  LoadContent();

  auto ground = World::CreateEntity("Ground");
  ground->SetComponent<Position>({{0.0f, -6.0f}});
  ground->SetComponent<Rotation>({0.0f});
  ground->SetComponent<Scale>({{40.0f, 2.0f}});
  ground->AddComponent<Rigidbody2D>();
  ground->AddComponent<BoxCollider2D>();
  ground->GetComponent<BoxCollider2D>()->HalfExtents = {20.0f, 1.0f};

  auto player = World::CreateEntity("Player");
  player->SetComponent<Position>({{0.0f, -5.0f}});
  player->SetComponent<Rotation>({0.0f});
  player->SetComponent<Scale>({{1.0f, 1.0f}});
  player->SetComponent<Controller>(
      {KeyCode::A, KeyCode::D, 5.0f});
  player->SetComponent<Sprite2D>({bucketTexture});
  player->AddComponent<Player>();
  player->AddComponent<Rigidbody2D>();
  auto playerRB = player->GetComponent<Rigidbody2D>();
  playerRB->LockRotation = true;
  playerRB->Type = Rigidbody2D::BodyType::Dynamic;
  player->AddComponent<CircleCollider2D>();

  World::BindSystem<const Controller, Rigidbody2D, const Position>(
      flecs::OnUpdate, "Move", Move);
}

Game *CreateGameInstance() 
{ 
    return new TemplateGame(); 
}
} // namespace BladeEngine
