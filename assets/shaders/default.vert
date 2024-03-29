#version 450

layout(binding = 0) uniform MVP{
  mat4 model;
  mat4 view;
  mat4 proj;
}
mvp;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoordinate;

layout(location = 0) out vec3 fragmentColor;
layout(location = 1) out vec2 fragmentTextureCoordinate;

void main() {
  gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inPosition, 1.0);
  fragmentColor = inColor;
  fragmentTextureCoordinate = inTextureCoordinate;
}
