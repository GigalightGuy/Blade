#pragma once

#include "../Core/Vec.hpp"

class b2World;

namespace BladeEngine
{
    struct Rigidbody2D;

    struct PhysicsMaterial
    {
        float Density = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;
    };

    class Physics2D
    {
    public:
        static int GetVelocityIterations() { return s_VelocityIterations; }
        static int GetPositionIterations() { return s_PositionIterations; }

        static void SetVelocityIterations(int value) { s_VelocityIterations = value; }
        static void SetPositionIterations(int value) { s_PositionIterations = value; }

        static void* GetPhysicsWorldHandle() { return s_PhysicsWorld; }

        static void AddImpulse(Rigidbody2D& rb, Vec2 direction, float strength);
        static void AddImpulse(Rigidbody2D& rb, Vec2 impulse);

        static void AddForce(Rigidbody2D& rb, Vec2 direction, float strength);
        static void AddForce(Rigidbody2D& rb, Vec2 force);

        static Vec2 GetBodyVelocity(Rigidbody2D& rb);
        static float GetBodyMass(Rigidbody2D& rb);

    private:
        static void Init();
        static void Shutdown();

    private:
        static int s_VelocityIterations;
        static int s_PositionIterations;

        static b2World* s_PhysicsWorld;

        friend class Game;

    };
}