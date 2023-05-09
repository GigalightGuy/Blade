#include "Texture2D.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GraphicsManager.hpp"

using namespace BladeEngine::Graphics;
Texture2D::Texture2D(std::string path) {
  int texChannels;

  pixels = stbi_load(path.c_str(), &width, &height, &texChannels, STBI_rgb_alpha);

  imageSize = width * height * 4;

  if (!pixels) {
    std::cout << "failed to load texture image at: " << path.c_str() << std::endl;
    throw std::runtime_error("failed to load texture image!");
  }
}

Texture2D::~Texture2D() 
{ 
  stbi_image_free(pixels);

  if (gpuTextureHandle)
  {
    DestroyGPUTexture();
  }
}

void Texture2D::CreateGPUTexture()
{
  gpuTextureHandle = GraphicsManager::Instance()->UploadTextureToGPU(this);
}

void Texture2D::DestroyGPUTexture()
{
  GraphicsManager::Instance()->ReleaseGPUTexture(gpuTextureHandle);
}
