#version 450

layout(binding = 0) uniform Camera{
  mat4 viewMatrix;
  mat4 projectionMatrix;
}camera;

layout(binding = 1) uniform Model{
  mat4 transformMatrix;
}model;

layout(location = 0) in vec3 inVertexPosition;
layout(location = 1) in vec2 inVertexTextureCoordinate;

layout(location = 0) out vec2 fragmentTextureCoordinate;

void main() {
  gl_Position = camera.projectionMatrix * camera.viewMatrix * model.transformMatrix * vec4(inVertexPosition, 1.0);
  fragmentTextureCoordinate = inVertexTextureCoordinate;
}
