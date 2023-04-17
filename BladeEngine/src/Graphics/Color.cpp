#include "Color.hpp"
#include <cstdint>

using namespace BladeEngine::Graphics;

Color::Color() 
{
    glm::vec4 color = CorrectColor(glm::vec4(0,0,0,255));    
    value = glm::vec4(color.r / 255, color.g / 255, color.b / 255, 1);
}

Color::Color(uint8_t r, uint8_t g, uint8_t b) 
{
    glm::vec4 color = CorrectColor(glm::vec4(r,g,b,255));    
    value = glm::vec4(color.r / 255, color.g / 255, color.b / 255, 1);
}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {    
    glm::vec4 color = CorrectColor(glm::vec4(r,g,b,a));    
    value = glm::vec4(color.r / 255, color.g / 255, color.b / 255, color.w/255);
}

glm::vec4 BladeEngine::Graphics::Color::GetValues()
{
    return value;
}


glm::vec4 BladeEngine::Graphics::Color::CorrectColor(glm::vec4 color)
{
    color.x = color.x > 255 ? 255 : color.x;
    color.x = color.x < 0 ? 0 : color.x;
    
    color.y = color.y > 255 ? 255 : color.y;
    color.y = color.y < 0 ? 0 : color.y;
    
    color.z = color.z > 255 ? 255 : color.z;
    color.z = color.z < 0 ? 0 : color.z;
    
    color.w = color.w > 255 ? 255 : color.w;
    color.w = color.w < 0 ? 0 : color.w;

    return color;    
}
