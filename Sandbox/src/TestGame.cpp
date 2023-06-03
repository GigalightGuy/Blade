#include "TestGame.hpp"

#include "BladeEngine.hpp"

#include "Graphics/SpriteSheet.hpp"
#include "Audio/AudioClip.hpp"
#include "Audio/AudioManager.hpp"
#include "Audio/AudioSource.hpp"
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

	Graphics::Texture2D* g_TextureChickBoy;
	Graphics::Texture2D* g_TexturePlatformBlock;
	std::vector<Graphics::Texture2D*> g_BackgroundTextures;

	Graphics::Texture2D* g_TexturePlayerIdle;
	Graphics::SpriteSheet* g_SpriteSheetPlayerIdle;

	Entity g_Ground;

	AudioClip* jumpClip;
	AudioClip* backgroundClip;
	AudioSource* audioSource;
	AudioSource* backgroundAudioSource;

	Graphics::Font* g_KrinkesRegularFont;
	Graphics::Font* g_AdmirationPainsFont;
	Graphics::Font* g_ZerafocFont;
	Graphics::Font* g_OpenSansRegular;

	void LoadTextures() {
		using namespace Graphics;

		Texture2D::SamplerConfiguration samplerConfig;
		samplerConfig.Filter = SamplerFilter::Nearest;
		samplerConfig.AdressMode = SamplerAddressMode::ClampToEdges;

		g_TextureChickBoy =
			new Texture2D("assets/sprites/Chick-Boy Free Pack/tile000.png");
		g_TextureChickBoy->SetSamplerConfiguration(samplerConfig);
		g_TextureChickBoy->CreateGPUTexture();

		g_TexturePlatformBlock =
			new Texture2D("assets/sprites/Sunny-land-assets-files/PNG/"
				"environment/props/block-big.png");
		g_TexturePlatformBlock->SetSamplerConfiguration(samplerConfig);
		g_TexturePlatformBlock->CreateGPUTexture();

		g_TexturePlayerIdle = new Texture2D("assets/sprites/Sunny-land-assets-files/PNG/spritesheets/player-idle.png");
		g_TexturePlayerIdle->SetSamplerConfiguration(samplerConfig);
		g_TexturePlayerIdle->CreateGPUTexture();

		g_SpriteSheetPlayerIdle = new SpriteSheet(g_TexturePlayerIdle, 33, 32);

		g_BackgroundTextures.resize(6);
		for (size_t i = 0; i < 6; i++) {
			std::stringstream path;
			path << "assets/sprites/PineForestParallax/MorningLayer" << i + 1 << ".png";

			Texture2D* tex = new Texture2D(path.str().c_str());
			g_BackgroundTextures[i] = tex;
			g_BackgroundTextures[i]->SetSamplerConfiguration(samplerConfig);
			g_BackgroundTextures[i]->CreateGPUTexture();
		}
	}

	void LoadAudioClips() {
		jumpClip = LoadAudioClip("assets/audio/jump.wav");
		audioSource = new AudioSource(jumpClip);

		backgroundClip = LoadAudioClip("assets/audio/backgroundmusic.mp3");
		backgroundAudioSource = new AudioSource(backgroundClip);
		backgroundAudioSource->SetLooping(true);
		backgroundAudioSource->Play();
	}

	void UnloadAudioClips() 
	{
		delete audioSource;
		delete backgroundAudioSource;
	}

	void UnloadTextures() 
	{
		delete g_TextureChickBoy;
		g_TextureChickBoy = nullptr;

		delete g_TexturePlatformBlock;
		g_TexturePlatformBlock = nullptr;

		delete g_TexturePlayerIdle;
		g_TexturePlayerIdle = nullptr;

		g_BackgroundTextures.clear();
	}

	void Move(
		flecs::entity e, 
		const Controller& ctrl, 
		Rigidbody2D& rb,
		const Position& pos) 
	{
		float desiredVel = 0.0f;
		desiredVel += Input::GetKey(ctrl.Left) ? -ctrl.Movespeed : 0.0f;
		desiredVel += Input::GetKey(ctrl.Right) ? ctrl.Movespeed : 0.0f;

		Vec2 currentVel = Physics2D::GetBodyVelocity(rb);
		float velChange = desiredVel - currentVel.X;
		float impulse = Physics2D::GetBodyMass(rb) * velChange;

		Physics2D::AddImpulse(rb, Vec2(impulse, 0.0f));

		RaycastHitInfo hitInfo;
		bool isGrounded =
			Physics2D::Raycast(*(g_Ground.GetComponent<Rigidbody2D>()), pos.Value,
				Vec2(0.0f, -1.0f), 0.55f, hitInfo);

		if (isGrounded && Input::GetKeyDown(ctrl.Jump)) {
			Physics2D::AddImpulse(rb, Vec2(0.0f, 1.0f), ctrl.JumpForce);
			audioSource->Play();
		}

		if (e.has<SpriteRenderer>())
		{
			auto sprite = e.get_mut<SpriteRenderer>();

			if (desiredVel < -0.01f)
				sprite->FlipX = true;
			else if (desiredVel > 0.01f)
				sprite->FlipX = false;
		}
	}

	void HandleOutOfMap(Position& pos) {
		if (pos.Value.Y < -10.0f)
			pos.Value = Vec2(Utils::Random::NextFloat(-10.0f, 10.0f), 5.0f);
	}

	void FocusCamera(const Player& player, const Position& pos) {
		Camera::GetMainCamera()->SetPosition({ pos.Value.X, 0.0f, 10.0f });
	}

	void GeneratePulse(const PulseEmitter& emitter, const Position& pos) {
		World::GetECSWorldHandle()->filter<Rigidbody2D, const Position>().each(
			[&](Rigidbody2D& rb, const Position& p) {
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

		//g_KrinkesRegularFont = new Graphics::Font("assets/fonts/KrinkesRegularPERSONAL.ttf");
		//g_AdmirationPainsFont = new Graphics::Font("assets/fonts/Admiration Pains.ttf");
		//g_ZerafocFont = new Graphics::Font("assets/fonts/Zerafoc.ttf");
		g_OpenSansRegular = new Graphics::Font("assets/fonts/open-sans/OpenSans-Regular.ttf");
	}

	void TestGame::UnloadGameResources() {
		UnloadTextures();
		// UnloadAudioClips();
	}

	void TestGame::SetupWorld() 
	{

		g_Ground = World::CreateEntity("Ground");
		g_Ground.SetComponent<Position>({ {0.0f, -5.0f} });
		g_Ground.SetComponent<Rotation>({ 0.0f });
		g_Ground.SetComponent<Scale>({ {40.0f, 2.0f} });
		g_Ground.AddComponent<Rigidbody2D>();
		g_Ground.AddComponent<BoxCollider2D>();
		g_Ground.GetComponent<BoxCollider2D>()->HalfExtents = { 20.0f, 1.0f };

		g_Ground.SetComponent<SpriteRenderer>({ g_TexturePlatformBlock });

		{
			auto chickBoy = World::CreateEntity("Chick Boy 1");
			chickBoy.SetComponent<Position>({ {-10.0f, 20.0f} });
			chickBoy.SetComponent<Rotation>({ 0.0f });
			chickBoy.SetComponent<Scale>({ {1.0f, 1.0f} });
			chickBoy.AddComponent<Rigidbody2D>();
			auto chickBoyRB = chickBoy.GetComponent<Rigidbody2D>();
			chickBoyRB->LockRotation = true;
			chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
			chickBoy.AddComponent<BoxCollider2D>();

			chickBoy.SetComponent<SpriteRenderer>({ g_TextureChickBoy });
		}

		{
			auto chickBoy = World::CreateEntity("Chick Boy 2");
			chickBoy.SetComponent<Position>({ {-5.0f, 20.0f} });
			chickBoy.SetComponent<Rotation>({ 0.0f });
			chickBoy.SetComponent<Scale>({ {1.0f, 1.0f} });
			chickBoy.AddComponent<Rigidbody2D>();
			auto chickBoyRB = chickBoy.GetComponent<Rigidbody2D>();
			chickBoyRB->LockRotation = true;
			chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
			chickBoy.AddComponent<BoxCollider2D>();

			chickBoy.SetComponent<SpriteRenderer>({ g_TextureChickBoy });
		}

		{
			auto chickBoy = World::CreateEntity("Chick Boy 3");
			chickBoy.SetComponent<Position>({ {5.0f, 20.0f} });
			chickBoy.SetComponent<Rotation>({ 0.0f });
			chickBoy.SetComponent<Scale>({ {1.0f, 1.0f} });
			chickBoy.AddComponent<Rigidbody2D>();
			auto chickBoyRB = chickBoy.GetComponent<Rigidbody2D>();
			chickBoyRB->LockRotation = true;
			chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
			chickBoy.AddComponent<BoxCollider2D>();

			chickBoy.SetComponent<SpriteRenderer>({ g_TextureChickBoy });
		}

		{
			auto chickBoy = World::CreateEntity("Chick Boy 4");
			chickBoy.SetComponent<Position>({ {10.0f, 20.0f} });
			chickBoy.SetComponent<Rotation>({ 0.0f });
			chickBoy.SetComponent<Scale>({ {1.0f, 1.0f} });
			chickBoy.AddComponent<Rigidbody2D>();
			auto chickBoyRB = chickBoy.GetComponent<Rigidbody2D>();
			chickBoyRB->LockRotation = true;
			chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
			chickBoy.AddComponent<BoxCollider2D>();

			chickBoy.SetComponent<SpriteRenderer>({ g_TextureChickBoy });
		}

		// auto pulsator = World::CreateEntity("Pulsator");
		// pulsator->SetComponent<Position>({ { 0.0f, 0.0f } });
		// pulsator->AddComponent<PulseEmitter>();

		auto player = World::CreateEntity("Player");
		player.SetComponent<Position>({ {0.0f, 5.0f} });
		player.SetComponent<Rotation>({ 0.0f });
		player.SetComponent<Scale>({ {1.0f, 1.0f} });
		player.SetComponent<Controller>(
			{ KeyCode::A, KeyCode::D, KeyCode::Space, 4.0f, 5.0f });
		player.AddComponent<Player>();
		player.AddComponent<Rigidbody2D>();
		auto playerRB = player.GetComponent<Rigidbody2D>();
		playerRB->LockRotation = true;
		playerRB->Type = Rigidbody2D::BodyType::Dynamic;
		player.AddComponent<CircleCollider2D>();

		player.SetComponent<SpriteRenderer>({ g_TextureChickBoy });

		player.AddComponent<SpriteAnimator>();
		auto anim = player.GetComponent<SpriteAnimator>();
		
		SpriteAnimation idle;
		idle.Frames = g_SpriteSheetPlayerIdle->GetFrames();
		idle.FrameDuration = 1 / 6.0f;

		anim->AnimationsMap["Idle"] = idle;
		anim->CurrentAnimation = &anim->AnimationsMap["Idle"];


		auto someText = World::CreateEntity("My First Text");
		someText.SetComponent<Position>({ { -10.0f, 5.0f } });
		someText.SetComponent<Rotation>({ 0.0f });
		someText.SetComponent<Scale>({ { 2.0f, 2.0f } });
		//someText.SetComponent<Sprite2D>({ g_ZerafocFont->GetAtlasTexture() });
		someText.SetComponent<TextRenderer>({ g_OpenSansRegular, "Hello, World!" });


		/*for (size_t i = 0; i < 6; i++) {
			std::stringstream ss("Background Layer ");
			ss << i + 1;

			auto background = World::CreateEntity(ss.str());
			background.SetComponent<Position>({ {0.0f, 0.0f} });
			background.SetComponent<Rotation>({ 0.0f });
			background.SetComponent<Scale>({ {100.0f, 50.0f} });

			background.SetComponent<Sprite2D>({ g_BackgroundTextures[i] });
		}*/

		World::BindSystem<const PulseEmitter, const Position>(2.0f, "Generate Pulse",
			GeneratePulse);

		World::BindSystem<const Controller, Rigidbody2D, const Position>(
			flecs::OnUpdate, "Move", Move);
		World::BindSystem<Position>(flecs::OnUpdate, "Handle out of Map",
			HandleOutOfMap);

		World::BindSystem<const Player, const Position>(flecs::PostUpdate,
			"Focus Camera", FocusCamera);
	}

	Game* CreateGameInstance() { return new TestGame(); }
} // namespace BladeEngine
