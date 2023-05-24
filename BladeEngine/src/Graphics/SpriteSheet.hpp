#pragma once

#include "../Core/Vec.hpp"
#include "../Components/Components.hpp"
#include "Texture2D.hpp"

#include <vector>

namespace BladeEngine::Graphics {

	class SpriteSheet
	{
	public:
		struct Section
		{
			Vec2 StartPos;
			Vec2 Dimensions;
		};

	public:
		SpriteSheet(Texture2D* texture, uint32_t spriteWidth, uint32_t spriteHeight);
		~SpriteSheet();

		void Subdivide(uint32_t spriteWidth, uint32_t spriteHeight);

		Texture2D* GetTexture() { return m_Texture; }

		std::vector<SpriteRenderer> GetFrames();
		std::vector<SpriteRenderer> GetFrames(uint32_t first, uint32_t last);
		std::vector<SpriteRenderer> GetFrames(std::vector<uint32_t> indices);

	private:
		Texture2D* m_Texture = nullptr;

		uint32_t m_RowsCount = 0, m_ColumnsCount = 0;
		std::vector<Section> m_Sections;

	};

}