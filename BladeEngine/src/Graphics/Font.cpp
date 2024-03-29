#include "Font.hpp"

#include "../Core/Base.hpp"

#include "MSDFData.hpp"


namespace BladeEngine::Graphics {

	static Texture2D* CreateAndCacheAtlas( 
		const std::vector<msdf_atlas::GlyphGeometry>& glyphs,
		uint32_t width, uint32_t height)
	{
		msdf_atlas::GeneratorAttributes attributes;
		attributes.config.overlapSupport = true;
		attributes.scanlinePass = true;

		msdf_atlas::ImmediateAtlasGenerator<float, 4, msdf_atlas::mtsdfGenerator, msdf_atlas::BitmapAtlasStorage<uint8_t, 4>> generator(width, height);
		generator.setAttributes(attributes);
		generator.setThreadCount(4);
		generator.generate(glyphs.data(), (int)glyphs.size());

		msdfgen::BitmapConstRef<uint8_t, 4> bitmap = (msdfgen::BitmapConstRef<uint8_t, 4>)generator.atlasStorage();
		
		// inverting image pixels on Y axis
		// TODO(Pedro): change this into an option to flip images
		uint8_t* pixels = new uint8_t[bitmap.width * bitmap.height * 4];
		for (int y = 0; y < bitmap.height; y++)
		{
			memcpy(&pixels[bitmap.width * 4 * y], 
				bitmap(0, bitmap.height - y - 1), 
				4 * bitmap.width);
		}


		Texture2D* texture = new Texture2D(bitmap.width, bitmap.height, TextureFormat::RGBA8);
		texture->SetData((void*)pixels, bitmap.width * bitmap.height * 4);

		Texture2D::SamplerConfiguration samplerConfig;
		samplerConfig.AdressMode = SamplerAddressMode::ClampToEdges;
		samplerConfig.Filter = SamplerFilter::Linear;
		texture->SetSamplerConfiguration(samplerConfig);

		texture->CreateGPUTexture();

		return texture;
	}

	Font::Font(const char* filepath)
		: m_Data(new MSDFData())
	{
		msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
		BLD_CORE_ASSERT(ft, "Failed to initialize FreetypeHandle");

		msdfgen::FontHandle* font = msdfgen::loadFont(ft, filepath);
		if (!font)
		{
			BLD_CORE_ERROR("Failed to load font: {}", filepath);
			return;
		}

		struct CharsetRange
		{
			uint32_t Begin, End;
		};

		static const CharsetRange charsetRanges[] =
		{
			{ 0x0020, 0x00FF }
		};

		msdf_atlas::Charset charset;
		for (CharsetRange range : charsetRanges)
		{
			for (uint32_t c = range.Begin; c <= range.End; c++)
				charset.add(c);
		}

		double fontScale = 1.0;
		m_Data->FontGeometry = msdf_atlas::FontGeometry(&m_Data->Glyphs);
		int glyphsLoaded = m_Data->FontGeometry.loadCharset(font, fontScale, charset);
		BLD_CORE_INFO("Loaded {} glyphs from font (out of {})", glyphsLoaded, charset.size());


		double emSize = 80.0;

		msdf_atlas::TightAtlasPacker atlasPacker;
		atlasPacker.setPixelRange(2.0);
		atlasPacker.setMiterLimit(1.0);
		atlasPacker.setPadding(0);
		atlasPacker.setMinimumScale(emSize);
		int remaining = atlasPacker.pack(m_Data->Glyphs.data(), (int)m_Data->Glyphs.size());
		BLD_CORE_ASSERT(remaining == 0, "Atlas packer failed to pack glyphs");

		int width, height;
		atlasPacker.getDimensions(width, height);
		emSize = atlasPacker.getScale();

#define DEFAULT_ANGLE_THRESHOLD 3.0
#define LCG_MULTIPLIER 6364136223846793005ull
#define LCG_INCREMENT 1442695040888963407ull

		unsigned long long glyphSeed = 0;
		for (msdf_atlas::GlyphGeometry& glyph : m_Data->Glyphs)
		{
			glyphSeed *= LCG_MULTIPLIER;
			glyph.edgeColoring(msdfgen::edgeColoringInkTrap, DEFAULT_ANGLE_THRESHOLD, glyphSeed);
		}

		m_AtlasTexture = CreateAndCacheAtlas(m_Data->Glyphs, width, height);


		msdfgen::destroyFont(font);
		msdfgen::deinitializeFreetype(ft);

	}

	Font::~Font()
	{
		delete m_Data;
		delete m_AtlasTexture;
	}

}