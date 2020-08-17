#pragma once
#include <core.h>
#include <Logger.h>

namespace xb2at {
namespace core {

	// All of the formats we need to care about.
	enum class TextureFormat : uint16 {
		R8_UNORM = 0x0201,
		R4G4B4A4_UNORM = 0x0301,
		R5G5B5A1_UNORM = 0x0501,
		B5G6R5_UNORM = 0x0701,
		R8A8_UNORM = 0x0901,
		R8A8_SNORM = 0x0902,
		R16_UNORM = 0x0A01,
		R8G8B8A8_UNORM = 0x0B01,
		R8G8B8A8_UNORM_SRGB = 0x0B06,
		R11G11B10_FLOAT = 0x0F05,
		R32_FLOAT = 0x1405,
		D32_FLOAT_S8X24_UINT = 0x1505,
		BC1_UNORM = 0x1A01,
		BC1_UNORM_SRGB = 0x1A06,
		BC2_UNORM = 0x1B01,
		BC2_UNORM_SRGB = 0x1B06,
		BC3_UNORM = 0x1C01,
		BC3_UNORM_SRGB = 0x1C06,
		BC4_UNORM = 0x1D01,
		BC4_SNORM = 0x1D02,
		BC5_UNORM = 0x1E01,
		BC5_SNORM = 0x1E02,
		BC6H_SF16 = 0x1F05,
		BC6H_UF16 = 0x1F0A,
		BC7_UNORM = 0x2001,
		BC7_UNORM_SRGB = 0x2006,

		ASTC_4x4_UNORM = 0x2D01,
		ASTC_4x4_SRGB = 0x2D06, // unused?
		ASTC_5x5_UNORM = 0x2F01,
		ASTC_8x8_UNORM = 0x3401
	};

	/**
	 * A Tegra X1 texture, that can be deswizzled.
	 */
	struct TegraX1SwizzledTexture {

		inline TegraX1SwizzledTexture() {

		}

		inline TegraX1SwizzledTexture(std::vector<byte>& SwizzledData, TextureFormat TexFormat, uint32 width, uint32 height) 
			: Format(TexFormat),
			width(width), 
			height(height) {

			Data.resize(SwizzledData.size());
			memcpy(&Data[0], &SwizzledData[0], SwizzledData.size() * sizeof(byte));

		}


		uint32 GetBpp();
		uint32 GetBlockWidth();
		uint32 GetBlockHeight();


		std::vector<byte> Data;
		TextureFormat Format;
		
		uint32 width;
		uint32 height;

		/**
		 * true if texture was deswizzled successfully. 
		 */
		bool deswizzled = false;

		void Deswizzle();

	private:


		// just in case
		Logger logger = Logger::CreateChannel("TegraTexture");
	};


}
}