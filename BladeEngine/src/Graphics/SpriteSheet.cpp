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
				section.Dimensions = Vec2(sectionSize.X, sectionSize.Y);
			}
		}
	}

	std::vector<SpriteRenderer> SpriteSheet::GetFrames()
	{
		return GetFrames(0, m_Sections.size() - 1);
	}

	std::vector<SpriteRenderer> SpriteSheet::GetFrames(uint32_t first, uint32_t last)
	{
		std::vector<uint32_t> indices(last - first + 1);

		for (size_t i = 0; i < indices.size(); i++)
		{
			indices[i] = i + first;
		}

		return GetFrames(indices);
	}

	std::vector<SpriteRenderer> SpriteSheet::GetFrames(std::vector<uint32_t> indices)
	{
		std::vector<SpriteRenderer> frames(indices.size());

		for (size_t i = 0; i < frames.size(); i++)
		{
			uint32_t sectionIndex = indices[i];

			if (sectionIndex >= m_Sections.size()) continue;

			auto& frame = frames[i];
			frame.Texture = m_Texture;
			
			frame.UVStartPos = m_Sections[sectionIndex].StartPos;
			frame.UVDimensions = m_Sections[sectionIndex].Dimensions;
		}

		return frames;
	}

}