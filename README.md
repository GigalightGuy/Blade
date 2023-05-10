# Blade GDK

## Blade GDK Documentation

# About
Blade GDK is a ***Game Development Kit*** focused on 2D Games for Windows and Linux x86 machines.
It uses the Vulkan API for Graphics Rendering and modern ECS concepts with the [Flecs](https://github.com/SanderMertens/flecs) project by [Sander Mertens](https://github.com/SanderMertens) 

# Requirements
- [Vulkan SDK](https://vulkan.lunarg.com/)
- [Visual Studio 2022](https://code.visualstudio.com/)

# Getting Started

To jump right into writing game code, you can jump into EmptyGame.cpp.  

The default content of EmptyGame.cpp has 3 empty functions:  
  
```cpp
#include "EmptyGame.hpp"

#include "BladeEngine.hpp"

namespace BladeEngine 
{

	// Load your game assets inside this function
	void ExampleGame::LoadGameResources()
	{
	}

	// Unload your game assets inside this function
	void ExampleGame::UnloadGameResources()
	{
	}

	// Initialize your game world state inside this function
	// Entity creation and system binding goes in here
	void ExampleGame::SetupWorld()
	{
	}

}
```

These 3 functions are called from engine side.  
`LoadGameResources` should be used to load any external files needed for your game such as textures, audio clips, custom shaders, etc... 
If you load your own assets to the game, you should also unload them when your finished and that's why you have `UnloadGameResources`.  
`LoadGameResources` is called by the engine when you start your game and `UnloadGameResources` is called when you are exiting the game.  

`SetupWorld` is called by the engine after initialization and loading of resources right before the gameplay starts.
Its purpose is for the developer to have somewhere where they can layout their game world and their game logic.

To get started, you can load a texture inside the `LoadGameResources` like this:  
  
```cpp
Graphics::Texture2D* rainDropTexture;

// Load your game assets inside this function
void ExampleGame::LoadGameResources()
{
	rainDropTexture = new Graphics::Texture2D("assets/blade.png");
}
```

This reads the texture data from a image file and stores that data inside an object of type `Texture2D`.  

Now we have the texture we wanted to use inside our game, but if we want to use ir for rendering, we still need to get this texture data into the GPU.  
For that, we need to add the following line after creating our `Texture2D` object:  

```cpp
// Load your game assets inside this function
void ExampleGame::LoadGameResources()
{
	bladeTexture = new Graphics::Texture2D("assets/blade.png");
	bladeTexture->CreateGPUTexture(); // New line of code
}
```

This is gonna create a texture in GPU with the data we loaded into our `Texture2D`. 

If we are creating a texture in the GPU, it is probably a good idea to also get rid of it before we exit our game. 
Let's use `UnloadGameResources` for that: 
 
```cpp
void ExampleGame::UnloadGameResources()
{
	bladeTexture->DestroyGPUTexture();
}
```
Great, we now have a texture in GPU and we also get rid of it when we don't need it anymore! 
Everything is in place for us to get something drawn to the screen. 
 
We are finally gonna use the `SetupWorld` function to... well, set up our world! 
 
Objects that reside inside our game world are called **Entities**, but they actually are just empty shells,
**Components** are what give them some meaning. 
 
A **Component** is just a simple data container (struct) with no logic attached to it. 
An **Entity** can have multiple **Components** associated with it. 
 
Now that we know what are **Entities** and **Components**, let's create our first Entity: 
```cpp
void ExampleGame::SetupWorld()
{
	Entity bladeEntity = World::CreateEntity("Blade");
}
```
Done. Simple, ain't it? 

Here we are calling `World::CreateEntity`, meaning we are asking the class that manages our game world to create us an **Entity**. 
The text we are passing as parameter to `CreateEntity` is an optional name for the **Entity** we are creating. 

Now we have our first ever **Entity** in our game world, but remember what I said about **Entities** having no use if no components are attached to it. 
Our **Entity** is just an identifier, without any **Components** associated with it, it can not hold any meaningful data. Let's solve that: 
```cpp
void ExampleGame::SetupWorld()
{
	Entity bladeEntity = World::CreateEntity("Blade");
	
	bladeEntity.SetComponent<Sprite2D>({ bladeTexture });
}
```
`SetComponent<T>` can be used to set the data of a **Component** of type `T`,
if the **Entity** does not yet have a **Component** of type `T`,
this function will also add a component of said type to the **Entity**. 

`Sprite2D` is a default **Component** type that takes a `Texture2D` reference and also works as a hint to tell the engine we want this **Entity** to get rendered. 
When I say render the **Entity**, I actually mean rendering a qaud with the `Texture2D` that is referenced by the `Sprite2D` **Component**. 

Now that we gave our **Entity** a `Sprite2D` **Component**, it should have everything it needs to get drawn to the screen!... right? 
Well, we are indeed informing the engine we want to draw a quad with a certain texture, but it still wants some more information. 

Don't worry, it's not much. 
The engine just wants to know where the **Entity** is located in the game world, so it knows where to draw it and also the its rotation and size. 

Let's get to it: 
```cpp
void ExampleGame::SetupWorld()
{
	...
	
	bladeEntity.SetComponent<Sprite2D>({ bladeTexture });
	
	// New code
	bladeEntity.SetComponent<Position>({ { 0.0f, 0.0f } });
	bladeEntity.SetComponent<Rotation>({ 0.0f });
	bladeEntity.SetComponent<Scale>({ { 1.0f, 1.0f } });
	//---------
}
```
Once again we use `SetComponent<T>` to both add and set the values for our new **Components**.
`Position`, `Rotation` and `Scale` are all built in **Components**;

Both structs `Position` and `Scale` use the `Vec2` type to store 2 values ([Vec2](#vec2) also has some neat vector math functions to make your life easier!). 

Now if you run your code, you will finally get to see your textured quad in the center of the screen,
because the default camera is looking at position (0,0).

Don't forget to load an actual image inside `LoadGameResources` if you want to see the quad.

This **Getting Started** simple example is too simple and boring, we didn't even add any interactions to our game :(, but fear not,
if you want to see what else you can do with the **Blade Engine**, take a look at the [Example Project](#example-project) section and
you can always refer to the [Documentation](#documentation) if you ever are in doubt about API specifics.

### Example Project

### **Setup**

- Download an image of a bucket, a rain drop and a blue square texture and add them to the sprites directory

```
root/
    assets/
        sprites/
            bucket.png
            drop.png
            water.png
        shaders/
            default.vert
            default.frag
```
- Create a folder named src and in it add a Components and Systems directory.

```
root/
    assets/
        sprites/
            bucket.png
            drop.png
            water.png
        shaders/
            default.vert
            default.frag
    src/
        Components/
        Systems/
```

### **Main File**

- Create two new files named **ExampleGame.cpp** and **ExampleGame.hpp** and add them to the root of the src folder

```
root/
    ...
    src/
        Components/
        Systems/
        ExampleGame.cpp
        ExampleGame.hpp
```

Let's start by initiating to the .hpp file the following code

ExampleGame.hpp
```cpp
#pragma once

#include "Core/Game.hpp"

namespace BladeEngine
{
    class ExampleGame : public Game
    {
    private:
        virtual void SetupWorld() override;
    };
}
```

And let's do the same to the .cpp file

ExampleGame.cpp
```cpp
#include "ExampleGame.hpp"

#include "BladeEngine.hpp"

#include <sstream>

namespace BladeEngine {

void LoadContent()
{
    
}

void TemplateGame::SetupWorld() 
{
  LoadContent()
}

Game *CreateGameInstance() 
{ 
    return new TemplateGame(); 
}
}
```

### **Drawing Textures**

- To draw textures, first we must load them in the LoadContent function and generate the textures to be used by the Project Renderer

ExampleGame.cpp
```cpp
#include "ExampleGame.hpp"

#include "BladeEngine.hpp"

#include <sstream>

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

void TemplateGame::SetupWorld() 
{
  LoadContent()
}

Game *CreateGameInstance() 
{ 
    return new TemplateGame(); 
}
}
```

- After Loading them, we must create the project Entities and add the Position,Rotation, Scale and Sprite2D Components to them as follows.

ExampleGame.cpp
```cpp
#include "ExampleGame.hpp"

#include "BladeEngine.hpp"

#include <sstream>

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

void TemplateGame::SetupWorld() 
{
  LoadContent()

  auto raindrop = World::CreateEntity("Raindrop");
  raindrop->SetComponent<Position>({{0.0f, 10.0f}});
  raindrop->SetComponent<Rotation>({0.0f});
  raindrop->SetComponent<Scale>({{1.0f, 1.0f}});
  raindrop->SetComponent<Sprite2D>({rainDropTexture});

  auto player = World::CreateEntity("Player");
  player->SetComponent<Position>({{0.0f, -5.0f}});
  player->SetComponent<Rotation>({0.0f});
  player->SetComponent<Scale>({{1.0f, 1.0f}});
  player->SetComponent<Sprite2D>({bucketTexture});

  auto waterBar = World::CreateEntity("WaterBar");
  waterBar->SetComponent<Position>({{0.0f, -8.0f}});
  waterBar->SetComponent<Rotation>({0.0f});
  waterBar->SetComponent<Scale>({{1.0f, 1.0f}});
  waterBar->SetComponent<Sprite2D>({waterBarTexture});
}

Game *CreateGameInstance() 
{ 
    return new TemplateGame(); 
}
}
```
### **Add Gameplay**

- For the gameplay of this game, the Player will move horizontally in the game world while collecting rain drops. To do that we must create a Player structure to store the drops he collects and another to store our systems in the game logic.

- It is a good idea to have a way to move our player. For that, let's add another struct to store the Controller 

- Inside the Components directory, let's create a file named PlayerComponents.hpp and let's add the following to it.

```
root/
    ...
    src/
        Components/
            PlayerComponents.hpp
        Systems/
        ExampleGame.cpp
        ExampleGame.hpp
```

PlayerComponents.hpp
```cpp
#pragma once

#include "Core/Game.hpp"

struct Player {
  int points;
};

struct Controller {
  BladeEngine::KeyCode Left;
  BladeEngine::KeyCode Right;

  float Movespeed;
};
```

- And for the in the Systems directory, we add the PlayerSystems.hpp and PlayerSystems.cpp files

```
root/
    ...
    src/
        Components/
            PlayerComponents.hpp
        Systems/
            PlayerSystems.cpp
            PlayerSystems.hpp
        ExampleGame.cpp
        ExampleGame.hpp
```

- The Movement System of the player will allow us to move the bucket with input provided by the Controller component.

- Add the following code to the PlayerSystems.hpp

PlayerSystems.hpp
```cpp
#pragma once

#include "BladeEngine.hpp"
#include "../Components/PlayerComponents.hpp"

void Move(flecs::entity e, const Controller &ctrl, BladeEngine::Rigidbody2D &rb, const BladeEngine::Position &pos);
```
- And now, the implementation of this code in the PlayerSystems.cpp

PlayerSystems.cpp
```cpp
#include "PlayerSystems.hpp"

void Move(flecs::entity e, const Controller &ctrl, BladeEngine::Rigidbody2D &rb, const BladeEngine::Position &pos) 
{
  float desiredVel = 0.0f;
  desiredVel += BladeEngine::Input::GetKey(ctrl.Left) ? -ctrl.Movespeed : 0.0f;
  desiredVel += BladeEngine::Input::GetKey(ctrl.Right) ? ctrl.Movespeed : 0.0f;

  BladeEngine::Vec2 currentVel = BladeEngine::Physics2D::GetBodyVelocity(rb);
  float velChange = desiredVel - currentVel.X;
  float impulse = BladeEngine::Physics2D::GetBodyMass(rb) * velChange;

  BladeEngine::Physics2D::SetVelocity2D(rb,BladeEngine::Vec2(impulse, 0.0f),1);
}
```

- This system alone allows our Player entity to move horizontally physics forces, but to work we must add the components to the Player entity and bind the system to the ECS world systems of the game.

ExampleGame.cpp
```cpp
  auto player = World::CreateEntity("Player");
  player->SetComponent<Position>({{0.0f, -5.0f}});
  player->SetComponent<Rotation>({0.0f});
  player->SetComponent<Scale>({{1.0f, 1.0f}});
  player->SetComponent<Sprite2D>({bucketTexture});
  player->SetComponent<Controller>(
      {KeyCode::A, KeyCode::D, 5.0f});
  player->SetComponent<Player>({0});
  player->AddComponent<Rigidbody2D>();
  auto playerRB = player->GetComponent<Rigidbody2D>();
  playerRB->LockRotation = true;
  playerRB->Type = Rigidbody2D::BodyType::Dynamic;
  player->AddComponent<CircleCollider2D>();

  World::BindSystem<const Controller, Rigidbody2D, const Position>(flecs::OnUpdate, "Move", Move);
```

- We should also add a floor from where the player can move around in the world. Let's add anither entity called ground and let's have it's position right bellow the player.

ExampleGame.cpp
```cpp
  auto player = World::CreateEntity("Player");
  player->SetComponent<Position>({{0.0f, -5.0f}});
  player->SetComponent<Rotation>({0.0f});
  player->SetComponent<Scale>({{1.0f, 1.0f}});
  player->SetComponent<Sprite2D>({bucketTexture});
  player->SetComponent<Controller>(
      {KeyCode::A, KeyCode::D, 5.0f});
  player->SetComponent<Player>({0});
  player->AddComponent<Rigidbody2D>();
  auto playerRB = player->GetComponent<Rigidbody2D>();
  playerRB->LockRotation = true;
  playerRB->Type = Rigidbody2D::BodyType::Dynamic;
  player->AddComponent<CircleCollider2D>();

  World::BindSystem<const Controller, Rigidbody2D, const Position>(flecs::OnUpdate, "Move", Move);

  auto ground = World::CreateEntity("Ground");
  ground->SetComponent<Position>({{0.0f, -6.0f}});
  ground->SetComponent<Rotation>({0.0f});
  ground->SetComponent<Scale>({{40.0f, 2.0f}});
  ground->AddComponent<Rigidbody2D>();
  ground->AddComponent<BoxCollider2D>();
  ground->GetComponent<BoxCollider2D>()->HalfExtents = {20.0f, 1.0f};
```

### **Rain Drops**
- For the rain drops, we will have to make another component where we will store the drop value and if it is double points or not.

- Inside the Components directory, create the RaindropComponents.hpp file and add the following

RaindropComponents.hpp
```cpp
#pragma once

struct Raindrop{
    int value;
    bool doubleValue;
};
```

- Again, lets also add a new System file called RaindropSystems.hpp and RaindropSystems.cpp

- Inside the RaindropSystems.hpp let's add the following function declaration

RaindropSystems.hpp
```cpp
#pragma once

#include "BladeEngine.hpp"
#include "../Components/RaindropComponents.hpp"

void RebootOnGroundHit(Raindrop &drop, BladeEngine::Position &pos,BladeEngine::Rigidbody2D &rb);
```

- And inside RaindropSystems.cpp let's add the implementation

RaindropSystems.cpp
```cpp
#include "RaindropSystems.hpp"
#include "../Components/PlayerComponents.hpp"
#include "../Components/GroundComponents.hpp"

#include "Utils/Random.hpp"
#include <iostream>

void RebootOnGroundHit(Raindrop &drop, BladeEngine::Position &pos, BladeEngine::Rigidbody2D &rb)
{
    BladeEngine::World::GetECSWorldHandle()->filter<Player,const BladeEngine::Position>().each(
      [&](Player& player ,const BladeEngine::Position& position) 
      {
        if((pos.Value - position.Value - BladeEngine::Vec2(0.0f,0.5f)).Length() < 0.5f)
        {
          pos.Value = BladeEngine::Vec2(BladeEngine::Utils::Random().NextFloat(-5,5),10);
          BladeEngine::Physics2D::SetVelocity2D(rb,BladeEngine::Vec2(0,-1),1);
          player.points += drop.value + (drop.doubleValue ? drop.value : 0);
          drop.doubleValue = false;
        }                
      });

    BladeEngine::World::GetECSWorldHandle()->filter<const Ground,const BladeEngine::Position>().each(
      [&](const Ground ground ,const BladeEngine::Position& position) 
      {
        if(pos.Value.Y - position.Value.Y < 1.6f)
        {
          drop.doubleValue = BladeEngine::Utils::Random().NextFloat() >= 0.2f ? false : true;
          pos.Value = BladeEngine::Vec2(BladeEngine::Utils::Random().NextFloat(-5,5),10);
          BladeEngine::Physics2D::SetVelocity2D(rb,BladeEngine::Vec2(0,-1),1);
        }                
      });
}
```

- Inside this function it uses the ECS handle of the world to filter all the players in the game and check if the drop is in colliding distance to consider a hit and add a point to it, if that doesn't confirm, we check the same to the ground. if the Player hits the drop, we get the drops point plus the double of that if we are lucky, and if the drop hits the ground, it randomly sets its double value to be true or false.

- To have this work though, we must first add some new Components and bind this system into the Game.

- Lets start by adding a GroundComponents file into the Components directory and have it as follows

GroundComponents.hpp
```cpp
#pragma once

struct Ground{};
```
PlayerComponents.hpp
```cpp
#pragma once

#include "Core/Game.hpp"

struct Player {
  int points;
};

struct WaterBar{};

struct Controller {
  BladeEngine::KeyCode Left;
  BladeEngine::KeyCode Right;

  float Movespeed;
};
``` 

- ExampleGame.cpp
```cpp
void TemplateGame::SetupWorld() 
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
}
```

### **User Interface**

- For us to keep track of the players progress, we will create a new System in the PlayerSystems files to control the image width acoording to the players points. For that, add the following to the PlayerSystems.hpp and PlayerSystems.cpp

PlayerSystems.hpp
```cpp
void UpdateUI(BladeEngine::Scale &scale, const WaterBar &waterBar);
```

PlayerSystems.cpp
```cpp
void UpdateUI(BladeEngine::Scale &scale, const WaterBar &waterBar)
{
   BladeEngine::World::GetECSWorldHandle()->filter<const Player>().each(
      [&](const Player& player) 
      {
        scale.Value.X = player.points*0.1f;                
      });
}
```
- Now let's add this system to the ExampleGame.cpp file and build the game

ExampleGame.cpp
```cpp
World::BindSystem<BladeEngine::Scale , const WaterBar>(flecs::OnUpdate, "UpdateUI", UpdateUI);
```

### **Build Project**

- To build the Game, create a CMakeLists.txt file in the root of the project and write the following

CMakeLists.txt
```cmake
add_executable(
    ExampleGame 
    src/ExampleGame.cpp 
    src/ExampleGame.hpp

    src/Components/PlayerComponents.hpp
    src/Systems/PlayerSystems.cpp
    src/Systems/PlayerSystems.hpp

    src/Components/RaindropComponents.hpp
    src/Systems/RaindropSystems.cpp
    src/Systems/RaindropSystems.hpp

    src/Components/GroundComponents.hpp
    )

target_link_libraries(ExampleGame PRIVATE BladeEngine)

target_include_directories(ExampleGame PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/src")
target_include_directories(ExampleGame PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/flecs") #TEMP
target_include_directories(ExampleGame PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/glm") #TEMP
target_include_directories(ExampleGame PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/spdlog/include")

set(ASSETS_SPRITES_DIR assets/sprites)
file(COPY ${ASSETS_SPRITES_DIR}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

		#set(VALIDATOR ${CMAKE_CURRENT_BINARY_DIR}/lib/BladeGDK/Lib/glslang/StandAlone/glslangValidator)
set(ASSETS_SHADERS_DIR assets/shaders)

file(GLOB files ${ASSETS_SHADERS_DIR}/*)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders)

# TODO: Find a way to compile glslangValidator before compiling shaders in CMAKE
foreach(file ${files})
		get_filename_component(FILENAME ${file} NAME)
		execute_process(
				COMMAND glslangValidator -V ${file} -o ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/${FILENAME}.spv
		)
endforeach()

target_compile_definitions(ExampleGame PRIVATE
    $<$<CONFIG:DEBUG>:BLADE_DEBUG>
    $<$<CONFIG:RELEASE>:BLADE_RELEASE>
    $<$<CONFIG:RELWITHDEBINFO>:BLADE_DEBUG>
    $<$<CONFIG:MINSIZEREL>:BLADE_RELEASE>)
```

- Now generate the cmake project, build it and run 

# Documentation

# Core

## Window

### Default Window System used with the [GLFW](https://github.com/glfw/glfw) window manager 

```cpp
Window(uint32_t width, uint32_t height, const std::string& title);
```
Get application window width.
```cpp
uint32_t GetWidth()
```

Get application window height.
```cpp
uint32_t GetHeight()
```

Get application aspect ratio.
```cpp
float GetViewportAspectRatio()
```

Call Operating System events to manage the window 
```cpp
void PollEvents()
```

Update the Input Key states
```cpp
void InputTick()
```
Get the required extensions of the Window Manager
```cpp
std::vector<const char*> GetRequiredExtensions()
```
## Vec2
### Struct that contains 2 floats: X, Y
```cpp
Vec2(float x, float y)
```
Calculates the squared length of this Vec2
```cpp
float SqrLength()
```

Calculates the length of this Vec2
```cpp
float Length()
```

Calculates the dot product of u and v
```cpp
float Dot(const Vec2& u, const Vec2& v)
```

Normalizes v
```cpp
Vec2 Normalize(Vec2 v)
```
## Vec3
### Struct that contains 3 floats: X, Y, Z
```cpp
Vec3(float x, float y, float z)
```
Calculates the squared length of this Vec3
```cpp
float SqrLength()
```

Calculates the length of this Vec3
```cpp
float Length()
```

Calculates the dot product of u and v
```cpp
float Dot(const Vec3& u, const Vec3& v)
```

Normalizes v
```cpp
Vec3 Normalize(Vec3 v)
```
Calculates the cross product of u and v
```cpp
Vec3 Cross(const Vec3& u, const Vec3& v)
```

## Utils
### static class with general use functions

Read file data into char vector
```cpp
static std::vector<char> ReadFile_vec(const std::string &filename);
```
Read file data into String
```cpp
static std::string ReadFile_str(const std::string &filename);
```

## Time
### static class to manage time passed in the executed program

Get World time in seconds
```cpp
CurrentWorldTime()
```

Get Elapsed time in seconds since last frame
```cpp
float DeltaTime()
```
## Input
### static class to Get Input Key states

Check if key is being held
```cpp
static bool GetKey(KeyCode key)
```

Check if mouse button is being held
```cpp
static bool GetMouseButton(MouseButton button)
```

Check if key was pressed this frame
```cpp
static bool GetKeyDown(KeyCode key)
```

Check if mouse button was pressed this frame
```cpp
static bool GetMouseButtonDown(MouseButton button)
```

Check if key was released this frame
```cpp
static bool GetKeyUp(KeyCode key)
```

Check if mouse button was released this frame
```cpp
static bool GetMouseButtonUp(MouseButton button)
```

Get the cursor position
```cpp
static std::pair<float, float> GetCursorPos() 
```

Get the cursor position x
```cpp
static float GetCursorX()
```

Get the cursor position y
```cpp
static float GetCursorY()
```

## Game
Base class for game projects
```cpp
Game()
```
Tells application to exit
```cpp
static void Exit()
```
## Camera
class to manage Camera instances
```cpp
Camera(float size, glm::vec2 position = glm::vec2(0.0f, 0.0f), float rotation = 0.0f,float nearPlane = 0.1f,float farPlane = 100.0f,CameraType cameraType = CameraType::ORTHOGRAPHIC);

Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f,0.0f), glm::vec3 rotation= glm::vec3(0.0f, 0.0f,0.0f),float nearPlane = 0.1f,float farPlane = 100.0f ,CameraType cameraType = CameraType::PERSPECTIVE);
```
Resize camera dimensions
```cpp
void Resize(glm::vec2 dimensions);
```
Set Camera Position
```cpp
void SetPosition(glm::vec2 position);

void SetPosition(glm::vec3 position);
```
Set Camera Rotation on the Z axis
```cpp
void SetRotation(float rotation);
```
Set Camera Rotation for all axis
```cpp
void SetRotation(glm::vec3 rotation);
```
Get Camera Aspect ratio
```cpp
float Aspect();
```
Get Camera Width
```cpp
float GetWidth();
```
Get Camera Height
```cpp
float GetHeight();
```
Get Camera Projection Matrix
```cpp
glm::mat4 GetProjectionMatrix();
```
Get Camera View Matrix
```cpp
glm::mat4 GetViewMatrix();
```
# ECS
### [Flecs](https://github.com/SanderMertens/flecs) Entity Component System used to make game logic in the Blade GDK game projects 

## World
### static class that contains and manages the ECS World and its entities

Creates a new Entity in the World
```cpp
static Entity* CreateEntity()
```           

Creates a new Entity in the World with a name
```cpp
static Entity* CreateEntity(const std::string& entityName)
```           

Destroys entity and removes it from World
```cpp
static void DestroyEntity(Entity* entity)
```           

Creates and binds a system to the World
```cpp
template<typename Func> static void BindSystemNoQuery(flecs::entity_t phase, const char* name, Func f)

template<typename ... Comps, typename Func> static void BindSystem(flecs::entity_t phase, const char* name, Func f)
        
template<typename ... Comps, typename Func> static void BindSystemIter(flecs::entity_t phase, const char* name, Func f)

template<typename ... Comps, typename Func> static void BindSystem(float timer, const char* name, Func f)
```           
## Entity

Creates and adds a new component of type T to this Entity
```cpp
template<typename T> inline void AddComponent()
```
Sets the data for the component of type T in this Entity
```cpp
template<typename T> inline void SetComponent(T&& value)
```
Removes component of type T from this Entity if it exists
```cpp
template<typename T> inline void RemoveComponent()
```
Get component of type T from this Entity if it exists
```cpp
template<typename T> inline const T* GetComponent()
```
Get this Entity's name
```cpp
inline const char* GetName()
```

## Graphics
Graphics wrapper using the Vulkan API for rendering 2D images

## Color
```cpp
Color(uint8_t r, uint8_t g, uint8_t b)
Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
```
Get the color value
```cpp
glm::vec4 GetValues()
```

## Mesh
```cpp
Mesh(std::vector<glm::vec3> vertices, std::vector<uint16_t> elements, std::vector<glm::vec2> uvs)
```
```cpp
Mesh Quad(float width,float height)
```

## ShaderType
```cpp
enum ShaderType {
  VERTEX,
  FRAGMENT,
  COMPUTE,
}
```

## Shader
```cpp
Shader(std::string path, ShaderType shaderType);
```

## Texture2D
```cpp
Texture2D(std::string path);
```
Create a Texture to be used by the Graphics Manager Renderer
```cpp
void CreateGPUTexture();
```
Destroys the generated Texture for the Graphics Manager Renderer
```cpp
void DestroyGPUTexture();
```
Gets the Graphics Manager Renderer Texture
```cpp
void* GetGPUTexture()
```

## VertexType
```cpp
enum VertexType 
{ 
    VERTEX_COLOR, 
    VERTEX_COLOR_TEXTURE, 
    VERTEX_TEXTURE 
}
```
## VertexColor
```cpp
struct VertexColor {
  glm::vec3 position;
  glm::vec4 color;
};
```

## VertexTexture
```cpp
struct VertexTexture {
  glm::vec3 position;
  glm::vec2 textureCoordinate;
};
```
## VertexColorTexture
```cpp
struct VertexColorTexture {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 textureCoordinate;
};
```



























