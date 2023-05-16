#include "TestGame.hpp"

#include "Audio/AudioClip.hpp"
#include "Audio/AudioManager.hpp"
#include "Audio/AudioSource.hpp"
#include "BladeEngine.hpp"
#include "Utils/Random.hpp"

#include <sstream>

namespace BladeEngine {
struct Controller {
  KeyCode Left, Right;
  KeyCode Jump;

  float Movespeed;
  float JumpForce;
};

struct PulseEmitter {
  float Strength = 10.0f;
  float Radius = 5.0f;
};

struct Player {};

Graphics::Texture2D *g_TextureChickBoy;
Graphics::Texture2D *g_TexturePlatformBlock;
std::vector<Graphics::Texture2D *> g_BackgroundTextures;

BladeEngine::Entity *g_Ground;

AudioClip *jumpClip;
AudioSource *audioSource;

// TODO: Move this to a load assets function in engine side
void LoadTextures() {
  g_TextureChickBoy =
      new Graphics::Texture2D("assets/sprites/Chick-Boy Free Pack/tile000.png");
  g_TextureChickBoy->CreateGPUTexture();

  g_TexturePlatformBlock =
      new Graphics::Texture2D("assets/sprites/Sunny-land-assets-files/PNG/"
                              "environment/props/block-big.png");
  g_TexturePlatformBlock->CreateGPUTexture();

  g_BackgroundTextures.resize(6);
  for (size_t i = 0; i < 6; i++) {
    std::stringstream path;
    path << "assets/sprites/PineForestParallax/MorningLayer" << i + 1 << ".png";

    Graphics::Texture2D *tex = new Graphics::Texture2D(path.str());
    g_BackgroundTextures[i] = tex;
    g_BackgroundTextures[i]->CreateGPUTexture();
  }
}

void LoadAudioClips() {
  jumpClip = LoadAudioClip("assets/audio/jump.wav");
  audioSource = new AudioSource(jumpClip);
}

void UnloadAudioClips() { delete audioSource; }

void UnloadTextures() {
  g_TextureChickBoy->DestroyGPUTexture();

  g_TexturePlatformBlock->DestroyGPUTexture();

  for (size_t i = 0; i < g_BackgroundTextures.size(); i++) {
    g_BackgroundTextures[i]->DestroyGPUTexture();
  }
}

void Move(flecs::entity e, const Controller &ctrl, Rigidbody2D &rb,
          const Position &pos) {
  float desiredVel = 0.0f;
  desiredVel += Input::GetKey(ctrl.Left) ? -ctrl.Movespeed : 0.0f;
  desiredVel += Input::GetKey(ctrl.Right) ? ctrl.Movespeed : 0.0f;

  Vec2 currentVel = Physics2D::GetBodyVelocity(rb);
  float velChange = desiredVel - currentVel.X;
  float impulse = Physics2D::GetBodyMass(rb) * velChange;

  Physics2D::AddImpulse(rb, Vec2(impulse, 0.0f));

  RaycastHitInfo hitInfo;
  bool isGrounded =
      Physics2D::Raycast(*(g_Ground->GetComponent<Rigidbody2D>()), pos.Value,
                         Vec2(0.0f, -1.0f), 0.55f, hitInfo);

  if (isGrounded && Input::GetKeyDown(ctrl.Jump)) {
    Physics2D::AddImpulse(rb, Vec2(0.0f, 1.0f), ctrl.JumpForce);
    audioSource->Play();
  }
}

void HandleOutOfMap(Position &pos) {
  if (pos.Value.Y < -10.0f)
    pos.Value = Vec2(Utils::Random::NextFloat(-10.0f, 10.0f), 5.0f);
}

void FocusCamera(const Player &player, const Position &pos) {
  Camera::GetMainCamera()->SetPosition({pos.Value.X, 2.0f, 10.0f});
}

void GeneratePulse(const PulseEmitter &emitter, const Position &pos) {
  World::GetECSWorldHandle()->filter<Rigidbody2D, const Position>().each(
      [&](Rigidbody2D &rb, const Position &p) {
        Vec2 pulseVec = p.Value - pos.Value;

        float distDiff = emitter.Radius - pulseVec.Length();
        if (distDiff < 0)
          return;

        float pulseForce = distDiff / emitter.Radius * emitter.Strength;
        Physics2D::AddImpulse(rb, Vec2(Normalize(pulseVec) * pulseForce));
      });
}

void TestGame::LoadGameResources() {
  LoadTextures();
  LoadAudioClips();
}

void TestGame::UnloadGameResources() {
  UnloadTextures();
  // UnloadAudioClips();
}

void TestGame::SetupWorld() {
  g_Ground = World::CreateEntity("Ground");
  g_Ground->SetComponent<Position>({{0.0f, 0.0f}});
  g_Ground->SetComponent<Rotation>({0.0f});
  g_Ground->SetComponent<Scale>({{40.0f, 2.0f}});
  g_Ground->AddComponent<Rigidbody2D>();
  g_Ground->AddComponent<BoxCollider2D>();
  g_Ground->GetComponent<BoxCollider2D>()->HalfExtents = {20.0f, 1.0f};

  g_Ground->SetComponent<Sprite2D>({g_TexturePlatformBlock});

  {
    auto chickBoy = World::CreateEntity("Chick Boy 1");
    chickBoy->SetComponent<Position>({{-10.0f, 20.0f}});
    chickBoy->SetComponent<Rotation>({0.0f});
    chickBoy->SetComponent<Scale>({{1.0f, 1.0f}});
    chickBoy->AddComponent<Rigidbody2D>();
    auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
    chickBoyRB->LockRotation = true;
    chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
    chickBoy->AddComponent<BoxCollider2D>();

    chickBoy->SetComponent<Sprite2D>({g_TextureChickBoy});
  }

  {
    auto chickBoy = World::CreateEntity("Chick Boy 2");
    chickBoy->SetComponent<Position>({{-5.0f, 20.0f}});
    chickBoy->SetComponent<Rotation>({0.0f});
    chickBoy->SetComponent<Scale>({{1.0f, 1.0f}});
    chickBoy->AddComponent<Rigidbody2D>();
    auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
    chickBoyRB->LockRotation = true;
    chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
    chickBoy->AddComponent<BoxCollider2D>();

    chickBoy->SetComponent<Sprite2D>({g_TextureChickBoy});
  }

  {
    auto chickBoy = World::CreateEntity("Chick Boy 3");
    chickBoy->SetComponent<Position>({{5.0f, 20.0f}});
    chickBoy->SetComponent<Rotation>({0.0f});
    chickBoy->SetComponent<Scale>({{1.0f, 1.0f}});
    chickBoy->AddComponent<Rigidbody2D>();
    auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
    chickBoyRB->LockRotation = true;
    chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
    chickBoy->AddComponent<BoxCollider2D>();

    chickBoy->SetComponent<Sprite2D>({g_TextureChickBoy});
  }

  {
    auto chickBoy = World::CreateEntity("Chick Boy 4");
    chickBoy->SetComponent<Position>({{10.0f, 20.0f}});
    chickBoy->SetComponent<Rotation>({0.0f});
    chickBoy->SetComponent<Scale>({{1.0f, 1.0f}});
    chickBoy->AddComponent<Rigidbody2D>();
    auto chickBoyRB = chickBoy->GetComponent<Rigidbody2D>();
    chickBoyRB->LockRotation = true;
    chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
    chickBoy->AddComponent<BoxCollider2D>();

    chickBoy->SetComponent<Sprite2D>({g_TextureChickBoy});
  }

  // auto pulsator = World::CreateEntity("Pulsator");
  // pulsator->SetComponent<Position>({ { 0.0f, 0.0f } });
  // pulsator->AddComponent<PulseEmitter>();

  auto player = World::CreateEntity("Player");
  player->SetComponent<Position>({{0.0f, 5.0f}});
  player->SetComponent<Rotation>({0.0f});
  player->SetComponent<Scale>({{1.0f, 1.0f}});
  player->SetComponent<Controller>(
      {KeyCode::A, KeyCode::D, KeyCode::Space, 4.0f, 5.0f});
  player->SetComponent<Sprite2D>({g_TextureChickBoy});
  player->AddComponent<Player>();
  player->AddComponent<Rigidbody2D>();
  auto playerRB = player->GetComponent<Rigidbody2D>();
  playerRB->LockRotation = true;
  playerRB->Type = Rigidbody2D::BodyType::Dynamic;
  player->AddComponent<CircleCollider2D>();

  for (size_t i = 0; i < 6; i++) {
    std::stringstream ss("Background Layer ");
    ss << i + 1;

    auto background = World::CreateEntity(ss.str());
    background->SetComponent<Position>({{0.0f, 0.0f}});
    background->SetComponent<Rotation>({0.0f});
    background->SetComponent<Scale>({{100.0f, 50.0f}});

    background->SetComponent<Sprite2D>({g_BackgroundTextures[i]});
  }

  World::BindSystem<const PulseEmitter, const Position>(2.0f, "Generate Pulse",
                                                        GeneratePulse);

  World::BindSystem<const Controller, Rigidbody2D, const Position>(
      flecs::OnUpdate, "Move", Move);
  World::BindSystem<Position>(flecs::OnUpdate, "Handle out of Map",
                              HandleOutOfMap);

  World::BindSystem<const Player, const Position>(flecs::PostUpdate,
                                                  "Focus Camera", FocusCamera);
}

Game *CreateGameInstance() { return new TestGame(); }
} // namespace BladeEngine
