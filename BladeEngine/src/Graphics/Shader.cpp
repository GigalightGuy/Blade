#include "Shader.hpp"
#include "../Core/Utils.hpp"

#include <vulkan/vulkan.h>

using namespace BladeEngine::Graphics;

Shader::Shader(std::string path, ShaderType shaderType) {
  data = Utils::ReadFile_vec(path + ".spv");
  type = shaderType;
}
Shader::~Shader() {}
