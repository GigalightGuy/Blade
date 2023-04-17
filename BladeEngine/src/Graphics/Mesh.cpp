#include "Mesh.hpp"

using namespace BladeEngine::Graphics;

Mesh::Mesh() {
  vertices = {
      {-0.5f, -0.5f, 0.0f},
      {0.5f, -0.5f, 0.0f},
      {0.5f, 0.5f, 0.0f},
      {-0.5f, 0.5f, 0.0f},
  };
  elements = {0, 1, 2, 2, 3, 0};
  uvs = {{1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};
}

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<uint16_t> elements,
           std::vector<glm::vec2> uvs) {
  this->vertices = vertices;
  this->elements = elements;
  this->uvs = uvs;
}

Mesh::~Mesh() {}

BladeEngine::Graphics::Mesh BladeEngine::Graphics::Quad(float width, float height)
{
    Mesh m;
    m.vertices = {
      {-width/2.0f, -height/2.0f, 1.0f},
      { width/2.0f, -height/2.0f, 1.0f},
      { width/2.0f,  height/2.0f, 1.0f},
      {-width/2.0f,  height/2.0f, 1.0f},
    };
    m.elements = {0, 1, 2, 2, 3, 0};
    m.uvs = {{1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}};
    return m;
}
