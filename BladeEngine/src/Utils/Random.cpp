#include "Random.hpp"

namespace BladeEngine::Utils
{
    std::random_device Random::s_FloatRandomDevice;
	std::mt19937 Random::s_FloatEngine(s_FloatRandomDevice());
	std::uniform_real_distribution<float> Random::s_FloatUniformDistribution(0, 1);

    std::random_device Random::s_IntRandomDevice;
    std::mt19937 Random::s_IntEngine(s_IntRandomDevice());
	std::uniform_int_distribution<int32_t> Random::s_IntUniformDistribution;

    float Random::NextFloat()
    {
        return s_FloatUniformDistribution(s_FloatEngine);
    }

    float Random::NextFloat(float min, float max)
    {
        return (max - min) * s_FloatUniformDistribution(s_FloatEngine) + min;
    }

    Vec2 Random::NextVec2()
    {
        return Vec2(NextFloat(), NextFloat());
    }

    Vec2 Random::NextVec2(float min, float max)
    {
        return Vec2(NextFloat(min, max), NextFloat(min, max));
    }

    Vec3 Random::NextVec3()
    {
        return Vec3(NextFloat(), NextFloat(), NextFloat());
    }

    Vec3 Random::NextVec3(float min, float max)
    {
        return Vec3(NextFloat(min, max), NextFloat(min, max), NextFloat(min, max));
    }
    
    int Random::NextInt()
    {
        return s_IntUniformDistribution(s_IntEngine);
    }
}