#pragma once

#include "../Core/Vec.hpp"
#include "../Physics/Physics2D.hpp"

class b2Body;

namespace BladeEngine::Graphics
{
    class Texture2D;
}

class BladeEngine::Graphics::Texture2D;

namespace BladeEngine
{
    struct Position
    {
        Vec2 Value;
    };

    struct Rotation
    {
        float Angle;
    };

    struct Scale
    {
        Vec2 Value;
    };

    struct Sprite2D
    {
        Graphics::Texture2D* Texture;
        Vec2 UVStartPos;
        Vec2 UVDimensions;
    };

    struct Rigidbody2D
    {
        enum class BodyType { Static = 0, Kinematic, Dynamic };

        BodyType Type = BodyType::Static;
        bool LockRotation = false;

        b2Body* RuntimeBody = nullptr;
    };

    enum PhysicsLayer : uint16_t
    {
        Layer_00 = 1 <<  0,
        Layer_01 = 1 <<  1,
        Layer_02 = 1 <<  2,
        Layer_03 = 1 <<  3,
        Layer_04 = 1 <<  4,
        Layer_05 = 1 <<  5,
        Layer_06 = 1 <<  6,
        Layer_07 = 1 <<  7,
        Layer_08 = 1 <<  8,
        Layer_09 = 1 <<  9,
        Layer_10 = 1 << 10,
        Layer_11 = 1 << 11,
        Layer_12 = 1 << 12,
        Layer_13 = 1 << 13,
        Layer_14 = 1 << 14,
        Layer_15 = 1 << 15,

        Layer_None = 0,
        Layer_All = 0b1111111111111111
    };
    

    struct BoxCollider2D
    {
        Vec2 HalfExtents = { 0.5f, 0.5f };

        PhysicsMaterial Material;

        bool IsSensor = false;

        PhysicsLayer CollisionLayers = Layer_00;
        PhysicsLayer CollisionMask = Layer_All;
        int16_t GroupId = 0;
    };

    struct CircleCollider2D
    {
        float Radius = 0.5f;

        PhysicsMaterial Material;

        bool IsSensor = false;

        PhysicsLayer CollisionLayers = Layer_00;
        PhysicsLayer CollisionMask = Layer_All;
        int16_t GroupId = 0;
    };

}