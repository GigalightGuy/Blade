#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace BladeEngine {
namespace Graphics {

enum ShaderType {
  VERTEX,
  FRAGMENT,
  COMPUTE,
};

class Shader {
public:
  Shader(std::string path, ShaderType shaderType);
  ~Shader();

  ShaderType type;
  std::vector<char> data;
};
} // namespace Graphics
} // namespace BladeEngine
