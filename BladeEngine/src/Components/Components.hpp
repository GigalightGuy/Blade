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

    struct BoxCollider2D
    {
        Vec2 HalfExtents = { 0.5f, 0.5f };

        PhysicsMaterial Material;
    };

    struct CircleCollider2D
    {
        float Radius = 0.5f;

        PhysicsMaterial Material;
    };

}