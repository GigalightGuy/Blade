#pragma once

class b2World;

namespace BladeEngine
{
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