#pragma once

#include "glm/glm.hpp"

#include "Texture.hpp"

namespace BladeEngine
{
    class Renderer2D
    {
        void DrawQuad(glm::mat4x4 transform, Texture2D texture);

    };

    
}