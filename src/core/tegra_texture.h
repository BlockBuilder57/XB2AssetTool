#pragma once
#include <core.h>

namespace xb2at {
namespace core {

	// All of the formats we need to care about.
	enum class TextureFormat : byte {
		R8_UNORM = 0x02,
		R4G4B4A4_UNORM = 0x03,
		R5G5B5A1_UNORM = 0x05,
		B5G6R5_UNORM = 0x07,
		R8A8_UNORM = 0x09,
		R8A8_SNORM = 0x09,
		R16_UNORM = 0x0A,
		R8G8B8A8_UNORM = 0x0B,
		R8G8B8A8_UNORM_SRGB = 0x0B,
		R11G11B10_FLOAT = 0x0F,
		R32_FLOAT = 0x14,
		D32_FLOAT_S8X24_UINT = 0x15,
		BC1_UNORM = 0x1A,
		BC1_UNORM_SRGB = 0x1A,
		BC2_UNORM = 0x1B,
		BC2_UNORM_SRGB = 0x1B,
		BC3_UNORM = 0x1C,
		BC3_UNORM_SRGB = 0x1C,
		BC4_UNORM = 0x1D,
		BC4_SNORM = 0x1D,
		BC5_UNORM = 0x1E,
		BC5_SNORM = 0x1E,
		BC6H_SF16 = 0x1F,
		BC6H_UF16 = 0x1F,
		BC7_UNORM = 0x20,
		BC7_UNORM_SRGB = 0x20 & 4,
		ASTC_4x4_UNORM = 0x2D,
		ASTC_5x5_UNORM = 0x2F,
		ASTC_8x8_UNORM = 0x34
	};


	struct TegraTexture {

		inline TegraTexture() {

		}

		inline TegraTexture(std::vector<byte>& TextureData, TextureFormat TexFormat, uint32 width, uint32 height, uint32 depth) 
			: Format(TexFormat),
			width(width), 
			height(height),
			depth(depth) {

			Data.resize(TextureData.size());
			memcpy(&Data[0], &TextureData[0], TextureData.size() * sizeof(byte));

		}

		std::vector<byte> Data;
		TextureFormat Format;
		
		uint32 width;
		uint32 height;
		uint32 depth;

		// In-place deswizzle this texture
		bool Deswizzle();
	};


}
}