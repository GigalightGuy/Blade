#pragma once

#include <cstdint>

namespace BladeEngine::Graphics {

	enum class TextureFormat
	{
		None = 0,
		R8,
		RG8,
		RGB8,
		RGBA8,
		RGBA32F
	};

	class Texture2D
	{
	public:
		Texture2D(uint32_t width, uint32_t height, TextureFormat format = TextureFormat::RGBA8);
		Texture2D(const char* path);
		~Texture2D();

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		TextureFormat GetFormat() const { return m_Format; }
		uint32_t GetBPP() const;

		uint32_t GetSize() const { return m_Width * m_Height * GetBPP(); }

		uint8_t* GetData() { return m_Pixels; }

		void SetData(void* pixels, uint32_t size);

		void CreateGPUTexture();
		void DestroyGPUTexture();

		inline void* GetGPUTexture() const { return m_GPUTextureHandle; }

	private:
		// Texture dimensions
		uint32_t m_Width, m_Height;

		TextureFormat m_Format;

		// Texture pixel data
		uint8_t* m_Pixels = nullptr;

		void* m_GPUTextureHandle = nullptr;

	};

} // namespace BladeEngine
