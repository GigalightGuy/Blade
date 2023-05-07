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