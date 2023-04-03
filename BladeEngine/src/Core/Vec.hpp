#pragma once

#include <iostream>

#include "Math.hpp"

namespace BladeEngine
{
    struct Vec2
    {
        float X, Y;

        Vec2(float x, float y) : X(x), Y(y) {}

        Vec2 operator-() const { return Vec2(-X, -Y); }

        Vec2& operator+=(const Vec2& other)
        {
            X += other.X;
            Y += other.Y;
            return *this;
        }

        Vec2& operator*=(const float t)
        {
            X *= t;
            Y *= t;
            return *this;
        }

        Vec2& operator/=(const float t)
        {
            return *this *= 1/t;
        }

        float SqrLength() const
        {
            return X * X + Y * Y;
        }

        float Length() const
        {
            return Math::Sqrt(SqrLength());
        }
    };

    // Vec2 Utility Functions

    inline std::ostream& operator<<(std::ostream& out, const Vec2& v)
    {
	    return out << v.X << ' ' << v.Y;
    }

    inline Vec2 operator+(const Vec2& u, const Vec2& v)
    {
        return Vec2(u.X + v.X, u.Y + v.Y);
    }

    inline Vec2 operator-(const Vec2& u, const Vec2& v)
    {
        return Vec2(u.X - v.X, u.Y - v.Y);
    }

    inline Vec2 operator*(const Vec2& u, const Vec2& v)
    {
        return Vec2(u.X * v.X, u.Y * v.Y);
    }

    inline Vec2 operator*(float t, const Vec2& v)
    {
        return Vec2(t * v.X, t * v.Y);
    }

    inline Vec2 operator*(const Vec2& v, float t)
    {
        return t * v;
    }

    inline Vec2 operator/(const Vec2& v, float t)
    {
        return (1/t) * v;
    }

    inline float Dot(const Vec2& u, const Vec2& v)
    {
        return u.X * v.X + u.Y * v.Y;
    }

    inline Vec2 Normalize(Vec2 v)
    {
	    return v / v.Length();
    }

    struct Vec3
    {
        float X, Y, Z;

        Vec3(float x, float y, float z) : X(x), Y(y), Z(z) {}

        Vec3 operator-() const { return Vec3(-X, -Y, -Z); }

        Vec3& operator+=(const Vec3& v)
        {
            X += v.X;
            Y += v.Y;
            Z += v.Z;
            return *this;
        }

        Vec3& operator*=(const float t)
        {
            X *= t;
            Y *= t;
            Z *= t;
            return *this;
        }

        Vec3& operator/=(const float t)
        {
            return *this *= 1/t;
        }

        float SqrLength() const
        {
            return X * X + Y * Y + Z * Z;
        }

        float Length() const
        {
            return Math::Sqrt(SqrLength());
        }
    };

    // Vec3 Utility Functions

    inline std::ostream& operator<<(std::ostream& out, const Vec3& v)
    {
	    return out << v.X << ' ' << v.Y << ' ' << v.Z;
    }

    inline Vec3 operator+(const Vec3& u, const Vec3& v)
    {
        return Vec3(u.X + v.X, u.Y + v.Y, u.Z + v.Z);
    }

    inline Vec3 operator-(const Vec3& u, const Vec3& v)
    {
        return Vec3(u.X - v.X, u.Y - v.Y, u.Z + v.Z);
    }

    inline Vec3 operator*(const Vec3& u, const Vec3& v)
    {
        return Vec3(u.X * v.X, u.Y * v.Y, u.Z * v.Z);
    }

    inline Vec3 operator*(float t, const Vec3& v)
    {
        return Vec3(t * v.X, t * v.Y, t * v.Z);
    }

    inline Vec3 operator*(const Vec3& v, float t)
    {
        return t * v;
    }

    inline Vec3 operator/(const Vec3& v, float t)
    {
        return (1/t) * v;
    }

    inline float Dot(const Vec3& u, const Vec3& v)
    {
        return u.X * v.X + u.Y * v.Y + u.Z * v.Z;
    }

    inline Vec3 Normalize(Vec3 v)
    {
	    return v / v.Length();
    }

    inline Vec3 Cross(const Vec3& u, const Vec3& v)
    {
        return Vec3(u.Y * v.Z - u.Z * v.Y,
				    u.Z * v.X - u.X * v.Z,
				    u.X * v.Y - u.Y * v.X);
    }
}