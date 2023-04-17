#pragma once

#include <array>
#include <glm/glm.hpp>

namespace BladeEngine {

namespace Graphics {
    
    enum VertexType
    {
      VERTEX_COLOR,
      VERTEX_COLOR_TEXTURE,
      VERTEX_TEXTURE
    };

struct VertexColor {
  glm::vec3 position;
  glm::vec4 color;
};

struct VertexTexture {
  glm::vec3 position;
  glm::vec2 textureCoordinate;
};

struct VertexColorTexture {
  glm::vec3 position;
  glm::vec4 color;
  glm::vec2 textureCoordinate;
};
} // namespace Graphics
} // namespace BladeEngine
