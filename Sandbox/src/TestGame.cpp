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

		bool IsGrounded = false;
	};

	struct MovingPlatform
	{
		Vec2 Anchor;
		Vec2 Movement;
		float Frequency = 1.0f;
	};

	struct OutOfMapCounter
	{
		int Count = 0;
	};

	struct FallCountText {};

	struct Player {};

	Graphics::Texture2D* g_TextureChickBoy;
	Graphics::Texture2D* g_TexturePlatformBlock;
	std::vector<Graphics::Texture2D*> g_BackgroundTextures;

	Graphics::Texture2D* g_TexturePlayerIdle;
	Graphics::SpriteSheet* g_SpriteSheetPlayerIdle;

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

		if (ctrl.IsGrounded && Input::GetKeyDown(ctrl.Jump)) {
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

	void MovePlatform(flecs::entity e, const MovingPlatform& movingPlatform, Position& pos)
	{
		pos.Value = movingPlatform.Anchor + 
			sinf(Time::CurrentWorldTime() * movingPlatform.Frequency) 
			* movingPlatform.Movement;
	}

	void HandleOutOfMap(Position& pos, OutOfMapCounter& counter)
	{
		if (pos.Value.Y < -10.0f)
		{
			pos.Value = Vec2(Utils::Random::NextFloat(-10.0f, 10.0f), 5.0f);
			counter.Count++;
		}
	}

	void UpdateFallCountText(TextRenderer& textRenderer, const OutOfMapCounter& counter)
	{
		textRenderer.Text = "Count: " + std::to_string(counter.Count);
	}

	void FocusCamera(const Player& player, const Position& pos) {
		Camera::GetMainCamera()->SetPosition({ pos.Value.X, 0.0f, 100.0f });
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


	class GroundedContactListener : public BaseBodyContactListener
	{
	public:
		GroundedContactListener() { }
		virtual ~GroundedContactListener() override { }

		virtual void BeginContact(BaseBodyContactListener* other) override
		{
			m_ContactCount++;
			GetEntity().get_mut<Controller>()->IsGrounded = m_ContactCount > 0;
		}

		virtual void EndContact(BaseBodyContactListener* other) override
		{
			m_ContactCount--;
			GetEntity().get_mut<Controller>()->IsGrounded = m_ContactCount > 0;
		}

	private:
		uint32_t m_ContactCount = 0;
	};

	void TestGame::SetupWorld() 
	{
		World::GetECSWorldHandle()->add<OutOfMapCounter>();

		auto blockPrefab = World::GetECSWorldHandle()->prefab("Platform Block")
			.set_override<Position>({ { 0.0f, 0.0f } })
			.set_override<Rotation>({ 0.0f })
			.set_override<Scale>({ { 1.0f, 1.0f } })
			.set_override<Rigidbody2D>({ })
			.add<BoxCollider2D>()
			.set<SpriteRenderer>({ g_TexturePlatformBlock });

		blockPrefab.get_mut<BoxCollider2D>()->HalfExtents = { 0.5f, 0.5f };

		auto largePlatform = World::GetECSWorldHandle()->prefab("Platform Large")
			.set_override<LocalToWorld>({ })
			.set_override<Position>({ {0.0f, 0.0f} })
			.set_override<Rotation>({ 0.0f })
			.set_override<Scale>({ {1.0f, 1.0f} })
			.set_override<Rigidbody2D>({ })
			.add<BoxCollider2D>();

		int numOfBlocks = 10;
		largePlatform.get_mut<BoxCollider2D>()->HalfExtents = { 0.5f * numOfBlocks, 0.5f };

			for (int i = 0; i < numOfBlocks; i++)
			{
				auto block1 = World::GetECSWorldHandle()->prefab()
					.child_of(largePlatform)
					.set_override<LocalToWorld>({ })
					.set_override<Position>({ { -numOfBlocks * 0.5f + 0.5f + i, 0.0f } })
					.set_override<Rotation>({ 0.0f })
					.set_override<Scale>({ {1.0f, 1.0f} })
					.set<SpriteRenderer>({ g_TexturePlatformBlock });
			}

		auto smallPlatform = World::GetECSWorldHandle()->prefab("Platform Small")
			.set_override<LocalToWorld>({ })
			.set_override<Position>({ {0.0f, 0.0f} })
			.set_override<Rotation>({ 0.0f })
			.set_override<Scale>({ {1.0f, 1.0f} })
			.set_override<Rigidbody2D>({ })
			.add<BoxCollider2D>();

		numOfBlocks = 3;
		smallPlatform.get_mut<BoxCollider2D>()->HalfExtents = { 0.5f * numOfBlocks, 0.5f };

			for (int i = 0; i < numOfBlocks; i++)
			{
				auto block1 = World::GetECSWorldHandle()->prefab()
					.child_of(smallPlatform)
					.set_override<LocalToWorld>({ })
					.set_override<Position>({ { -numOfBlocks * 0.5f + 0.5f + i, 0.0f } })
					.set_override<Rotation>({ 0.0f })
					.set_override<Scale>({ {1.0f, 1.0f} })
					.set<SpriteRenderer>({ g_TexturePlatformBlock });
			}
			

		World::GetECSWorldHandle()->entity().is_a(largePlatform)
			.set<Position>({ { -3.0f, 0.0f } });

		World::GetECSWorldHandle()->entity().is_a(smallPlatform)
			.set<Position>({ { 6.0f, 1.0f } });

		World::GetECSWorldHandle()->entity().is_a(smallPlatform)
			.set<MovingPlatform>({ {10.0f, 3.0f}, {3.0f, -1.0f}, 0.5f });

		World::GetECSWorldHandle()->entity().is_a(largePlatform)
			.set<Position>({ { 0.0f, 6.0f } });

		auto chickBoyPrefab = World::GetECSWorldHandle()->prefab("Chick Boy")
			.set<Position>({ { 0.0f, 0.0f } })
			.set<Rotation>({ 0.0f })
			.set<Scale>({ { 1.0f, 1.0f } })
			.set<SpriteRenderer>({ g_TextureChickBoy })
			.add<Rigidbody2D>()
			.add<BoxCollider2D>();

		auto chickBoyRB = chickBoyPrefab.get_mut<Rigidbody2D>();
		chickBoyRB->LockRotation = true;
		chickBoyRB->Type = Rigidbody2D::BodyType::Dynamic;
		chickBoyPrefab.get_mut<BoxCollider2D>()->HalfExtents = { 0.25f, 0.5f };

		for (int i = 0; i < 6; i++)
		{
			auto chickBoy = World::GetECSWorldHandle()->entity().is_a(chickBoyPrefab);
			chickBoy.add<LocalToWorld>();
			chickBoy.set<Position>({ { -12.0f + i * 4.0f, 20.0f } });
			chickBoy.set<Rigidbody2D>(*chickBoy.get<Rigidbody2D>());
		}
		
		auto player = World::GetECSWorldHandle()->entity("Player");
		player.add<LocalToWorld>();
		player.set<Position>({ {0.0f, 5.0f} });
		player.set<Rotation>({ 0.0f });
		player.set<Scale>({ {1.0f, 1.0f} });
		player.set<Controller>(
			{ KeyCode::A, KeyCode::D, KeyCode::Space, 4.0f, 5.0f });
		player.add<Player>();
		player.add<Rigidbody2D>();
		auto playerRB = player.get_mut<Rigidbody2D>();
		playerRB->LockRotation = true;
		playerRB->Type = Rigidbody2D::BodyType::Dynamic;
		playerRB->ContactListener = new GroundedContactListener();
		player.add<CircleCollider2D>();

		player.set<SpriteRenderer>({ g_TextureChickBoy });

		player.add<SpriteAnimator>();
		auto anim = player.get_mut<SpriteAnimator>();
		
		SpriteAnimation idle;
		idle.Frames = g_SpriteSheetPlayerIdle->GetFrames();
		idle.FrameDuration = 1 / 6.0f;

		anim->AnimationsMap["Idle"] = idle;
		anim->CurrentAnimation = &anim->AnimationsMap["Idle"];

		for (size_t i = 0; i < 6; i++) {
			std::stringstream ss("Background Layer ");
			ss << i + 1;

			auto background = World::CreateEntity(ss.str());
			background.SetComponent<Position>({ {0.0f, 0.0f} });
			background.SetComponent<Rotation>({ 0.0f });
			background.SetComponent<Scale>({ {100.0f, 50.0f} });
			background.AddComponent<LocalToWorld>();
			background.SetComponent<DepthSorting>({ -10.0f });

			background.SetComponent<SpriteRenderer>({ g_BackgroundTextures[i] });
		}


		auto someText = World::CreateEntity("Out of Map Count Text");
		someText.AddComponent<LocalToWorld>();
		someText.SetComponent<DepthSorting>({ 10.0f });
		someText.SetComponent<Position>({ { -15.0f, 7.0f } });
		someText.SetComponent<Rotation>({ 0.0f });
		someText.SetComponent<Scale>({ { 2.0f, 2.0f } });
		someText.SetComponent<TextRenderer>({ g_OpenSansRegular, "Count: 0" });
		someText.AddComponent<FallCountText>();

		World::BindSystem<const MovingPlatform, Position>(flecs::OnUpdate, "Move Platform", MovePlatform);
		World::BindSystem<const Controller, Rigidbody2D, const Position>(
			flecs::OnUpdate, "Move", Move);

		World::GetECSWorldHandle()->system<Position, OutOfMapCounter>("Handle out of Map")
			.term_at(2).singleton()
			.kind(flecs::OnUpdate)
			.each(HandleOutOfMap);

		World::GetECSWorldHandle()->system<TextRenderer, const OutOfMapCounter>("Update Count Text")
			.term_at(2).singleton()
			.term<FallCountText>()
			.kind(flecs::OnUpdate)
			.each(UpdateFallCountText);

		World::BindSystem<const Player, const Position>(flecs::PostUpdate,
			"Focus Camera", FocusCamera);
	}

	Game* CreateGameInstance() { return new TestGame(); }
} // namespace BladeEngine
