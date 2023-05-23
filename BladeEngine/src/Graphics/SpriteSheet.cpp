#include "SpriteSheet.hpp"

namespace BladeEngine::Graphics {



	SpriteSheet::SpriteSheet(Texture2D* texture, uint32_t spriteWidth, uint32_t spriteHeight)
		: m_Texture(texture)
	{
		Subdivide(spriteWidth, spriteHeight);
	}

	SpriteSheet::~SpriteSheet()
	{

	}

	void SpriteSheet::Subdivide(uint32_t spriteWidth, uint32_t spriteHeight)
	{
		uint32_t width = m_Texture->GetWidth(), height = m_Texture->GetHeight();

		m_RowsCount = width / spriteWidth;
		m_ColumnsCount = height / spriteHeight;

		Vec2 sectionSize{ (float)spriteWidth / width, (float)spriteHeight / height };

		m_Sections.resize(m_RowsCount * m_ColumnsCount);

		for (size_t i = 0, y = 0; y < m_ColumnsCount; y++)
		{
			for (size_t x = 0; x < m_RowsCount; x++, i++)
			{
				auto& section = m_Sections[i];
				section.StartPos = Vec2(sectionSize.X * x, sectionSize.Y * y);
			}
		}
	}

}