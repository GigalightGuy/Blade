# Blade GDK

## Blade GDK Documentation

# About
Blade GDK is a ***Game Development Kit*** focused on 2D Games for Windows and Linux x86 machines.
It uses the Vulkan API for Graphics Rendering and modern ECS concepts with the [Flecs](https://github.com/SanderMertens/flecs) project by [Sander Martens](https://github.com/SanderMertens) 

# Requirements
- [CMake](https://cmake.org/)
- [Vulkan SDK](https://vulkan.lunarg.com/)
- [GlslangValidator](https://github.com/KhronosGroup/glslang) as environment variable
- [Visual Studio Code](https://code.visualstudio.com/)

# Getting Started

Clone Blade Engine Project into your desired External Library folder and include the following lines into your CMakeLists.txt file. Exemple bellow consideres that the cloned project is in the folder named **external**.

```cmake
target_link_libraries(Template PRIVATE BladeEngine)

target_include_directories(Template PRIVATE "${CMAKE_SOURCE_DIR}/external/BladeEngine/src")
target_include_directories(Template PRIVATE "${CMAKE_SOURCE_DIR}/external/BladeEngine/vendor/flecs")
target_include_directories(Template PRIVATE "${CMAKE_SOURCE_DIR}/external/BladeEngine/vendor/glm")
target_include_directories(Template PRIVATE "${CMAKE_SOURCE_DIR}/external/BladeEngine/vendor/spdlog/include")
```

### Example Project

### **Setup**

- Create an assets folder with subdirectories named shaders and sprites

```
root/
    assets/
        sprites/
        shaders/
```

- Create the default shader files to be used in the rendering of the project, named default.vert and default.frag

```
root/
    assets/
        sprites/
        shaders/
            default.vert
            default.frag
```

default.vert
```glsl
#version 450

layout(binding = 0) uniform Camera{
  mat4 viewMatrix;
  mat4 projectionMatrix;
}camera;

layout(binding = 1) uniform Model{
  mat4 transformMatrix;
}model;

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec2 inVertexTextureCoordinate;

layout(location = 0) out vec2 fragmentTextureCoordinate;

void main() {
  gl_Position = camera.projectionMatrix * camera.viewMatrix * model.transformMatrix * vec4(inVertexPosition, 1.0);
  fragmentTextureCoordinate = inVertexTextureCoordinate;
}
```

default.frag
```glsl
#version 450

layout(binding = 2) uniform sampler2D texureSampler;

layout(location = 0) in vec2 fragmentTextureCoordinate;

layout(location = 0) out vec4 outColor;

void main() {
    
    outColor = texture(texureSampler, fragmentTextureCoordinate);
    
    if(outColor.a <= 0)
    {
        discard;
    }
}
```

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