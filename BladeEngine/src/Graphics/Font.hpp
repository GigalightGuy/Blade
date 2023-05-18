#pragma once

#include "Texture2D.hpp"

namespace BladeEngine::Graphics {

	struct MSDFData;

	class Font
	{
	public:
		Font(const char* font);
		~Font();

	private:
		MSDFData* m_Data;
		Texture2D* m_AtlasTexture;
	};

}