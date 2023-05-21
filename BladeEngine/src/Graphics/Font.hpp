#pragma once

#include "Texture2D.hpp"

namespace BladeEngine::Graphics {

	struct MSDFData;

	class Font
	{
	public:
		Font(const char* filepath);
		~Font();

		Texture2D* GetAtlasTexture() const { return m_AtlasTexture; }
		const MSDFData* GetMSDFData() const { return m_Data; }

	private:
		MSDFData* m_Data;
		Texture2D* m_AtlasTexture = nullptr;
	};

}