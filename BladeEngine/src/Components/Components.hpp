#pragma once

namespace BladeEngine
{
    struct Position
    {
        float X, Y;
    };

    struct Rotation
    {
        float Angle;
    };

    struct Scale
    {
        float X, Y;
    };

    struct Sprite2D
    {
        class Texture2D* Texture;
        float MinX, MinY;
        float LengthX, LengthY;
    };

}