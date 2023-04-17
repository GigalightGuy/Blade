#pragma once

#include "glm/glm.hpp"

namespace BladeEngine {
namespace Graphics {
class Color {
public:
  Color();
  Color(uint8_t r, uint8_t g, uint8_t b);
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

  glm::vec4 GetValues();
private:
  glm::vec4 CorrectColor(glm::vec4 color);
  glm::vec4 value;
};
} // namespace Graphics
} // namespace BladeEngine
