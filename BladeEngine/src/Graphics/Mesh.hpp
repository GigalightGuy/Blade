#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <iostream>
#include <optional>
#include <vector>

namespace BladeEngine {
namespace Graphics {
class Mesh {
public:
  Mesh();
  Mesh(std::vector<glm::vec3> vertices, std::vector<uint16_t> elements,
       std::vector<glm::vec2> uvs);
  ~Mesh();

  std::vector<glm::vec3> vertices;
  std::vector<uint16_t> elements;
  std::vector<glm::vec2> uvs;
};

Mesh Quad(float width, float height);
} // namespace Graphics
} // namespace BladeEngine
