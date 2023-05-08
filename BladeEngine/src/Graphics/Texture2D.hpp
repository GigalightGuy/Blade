#pragma once

#include <iostream>
#include <string>

namespace BladeEngine {
namespace Graphics {
class Texture2D {
public:
  // Given a Texture image path, load data to instance of texture 2D
  Texture2D(std::string path);
  // Free loaded pixel data
  ~Texture2D();

  void CreateGPUTexture();
  void DestroyGPUTexture();

  inline void *GetGPUTexture() const { return gpuTextureHandle; }

  // Texture width in pixels
  int width;
  // Texture height in pixels
  int height;
  // Texture image size in all channels (R,G,B,A)
  uint64_t imageSize;
  // Texture pixel data
  unsigned char *pixels;

  void *gpuTextureHandle;

private:
};
} // namespace Graphics
} // namespace BladeEngine
