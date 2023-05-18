#pragma once

#include "msdf-atlas-gen.h"

#include <vector>

namespace BladeEngine::Graphics {

	struct MSDFData
	{
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
		msdf_atlas::FontGeometry FontGeometry;
	};

}