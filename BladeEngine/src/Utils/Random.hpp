#pragma once

#include <random>

#include "../Core/Vec.hpp"

namespace BladeEngine::Utils 
{
    class Random
    {
    public:

        /**
        * Generates a new random float in the interval [0.0..1.0].
        *
        * @return generated random float.
        */
        static float NextFloat();

        /**
        * Generates a new random float in the interval [min..max].
        *
        * @param min lower limit of the interval where float is generated.
        * @param max upper limit of the interval where float is generated.
        * @return generated random float.
        */
        static float NextFloat(float min, float max);

        /**
        * Generates 2 new random floats in the interval [0.0..1.0].
        *
        * @return new Vec2 with the 2 generated floats in the X,Y members.
        */
        static Vec2 NextVec2();

        /**
        * Generates 2 new random floats in the interval [min..max].
        *
        * @param min lower limit of the interval where the values are generated.
        * @param max upper limit of the interval where the values are generated.
        * @return new Vec2 with the 2 generated floats in the X,Y members.
        */
        static Vec2 NextVec2(float min, float max);

        /**
        * Generates 3 new random floats in the interval [0.0..1.0].
        *
        * @return new Vec3 with the 3 generated floats in the X,Y,Z members.
        */
        static Vec3 NextVec3();

        /**
        * Generates 3 new random floats in the interval [min..max].
        *
        * @param min lower limit of the interval where the values are generated.
        * @param max upper limit of the interval where the values are generated.
        * @return new Vec3 with the 3 generated floats in the X,Y,Z members.
        */
        static Vec3 NextVec3(float min, float max);

        /**
        * Generates a new random int.
        *
        * @return generated random int.
        */
        static int NextInt();

    private:
        static std::random_device s_FloatRandomDevice;
	    static std::mt19937 s_FloatEngine;
	    static std::uniform_real_distribution<float> s_FloatUniformDistribution;

        static std::random_device s_IntRandomDevice;
        static std::mt19937 s_IntEngine;
	    static std::uniform_int_distribution<int32_t> s_IntUniformDistribution;
    };
}