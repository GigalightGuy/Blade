#include "Texture2D.hpp"

#include "../Core/Base.hpp"

#include "GraphicsManager.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace BladeEngine::Graphics {

	Texture2D::Texture2D(uint32_t width, uint32_t height, TextureFormat format)
		: m_Width(width), m_Height(height), m_Format(format)
	{
		
	}

	Texture2D::Texture2D(const char* path)
	{
		int width, height, texChannels;

		m_Pixels = stbi_load(path, &width, &height, &texChannels, STBI_rgb_alpha);

		m_Width = width;
		m_Height = height;
		m_Format = TextureFormat::RGBA8;

		BLD_CORE_ASSERT(m_Pixels, "Failed to load texture at {}", path);
	}

	Texture2D::~Texture2D()
	{
		if (m_Pixels)
		{
			stbi_image_free(m_Pixels);
		}

		if (m_GPUTextureHandle)
		{
			DestroyGPUTexture();
		}
	}

	uint32_t Texture2D::GetBPP() const
	{
		switch (m_Format)
		{
		case TextureFormat::None:
			return 0;
		case TextureFormat::R8:
			return 1;
		case TextureFormat::RG8:
			return 2;
		case TextureFormat::RGB8:
			return 3;
		case TextureFormat::RGBA8:
			return 4;
		case TextureFormat::RGBA32F:
			return 16;
		}

		BLD_CORE_ASSERT(false, "Unknown Texture Format");

		return 0;
	}

	void Texture2D::SetData(void* pixels, uint32_t size)
	{
		BLD_CORE_ASSERT(pixels, "Provided data is null");

		uint32_t bpp = GetBPP();
		BLD_CORE_ASSERT(size = m_Width * m_Height * bpp, "Provided data size does not match Texture2D size");

		m_Pixels = (uint8_t*)pixels;
	}

	void Texture2D::CreateGPUTexture()
	{
		m_GPUTextureHandle = GraphicsManager::Instance()->UploadTextureToGPU(this);
	}

	void Texture2D::DestroyGPUTexture()
	{
		GraphicsManager::Instance()->ReleaseGPUTexture(m_GPUTextureHandle);
	}

}