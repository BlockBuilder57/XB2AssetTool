#include <core.h>
#include <array>
#include "tegra_texture.h"

namespace xb2at {
namespace core {

	/**
	 * Structure used to store information
	 * about the supported formats in the support format table.
	 */
	struct TexFormatInfo {
		uint32 bpp;
		
		uint32 blockwidth;

		uint32 blockheight;

		uint32 blockdepth;
	};

	/**
	 * Constexpr array of all pairs of supported format in the layout of
	 *	{ ID, { INFO } }
	 */
	constexpr static std::array<std::pair<TextureFormat, TexFormatInfo>, 30> SupportedFormatData {{
		// format id, bpp, block width, block height, block depth
		{ TextureFormat::R8_UNORM,             {1,  1,  1, 1} },
		{ TextureFormat::R5G5B5A1_UNORM,       {2,  1,  1, 1} },
		{ TextureFormat::B5G6R5_UNORM,         {2,  1,  1, 1} },
		{ TextureFormat::R16_UNORM,            {2,  1,  1, 1} },
		{ TextureFormat::R8G8B8A8_UNORM,       {4,  1,  1, 1} },
		{ TextureFormat::R8G8B8A8_UNORM_SRGB,  {4,  1,  1, 1} },
		{ TextureFormat::R8G8B8A8_UNORM_SRGB,  {4,  1,  1, 1} },
		{ TextureFormat::R11G11B10_FLOAT,      {4,  1,  1, 1} },
		{ TextureFormat::R32_FLOAT,            {4,  1,  1, 1} },
		{ TextureFormat::D32_FLOAT_S8X24_UINT, {8,  1,  1, 1} },
		{ TextureFormat::BC1_UNORM,            {8,  4,  4, 1} },
		{ TextureFormat::BC1_UNORM_SRGB,       {8,  4,  4, 1} },
		{ TextureFormat::BC1_UNORM_SRGB,       {8,  4,  4, 1} },
		{ TextureFormat::BC2_UNORM,            {16, 4,  4, 1} },
		{ TextureFormat::BC2_UNORM_SRGB,       {16, 4,  4, 1} },
		{ TextureFormat::BC2_UNORM_SRGB,       {16, 4,  4, 1} },
		{ TextureFormat::BC3_UNORM,            {16, 4,  4, 1} },
		{ TextureFormat::BC3_UNORM_SRGB,       {16, 4,  4, 1} },
		{ TextureFormat::BC3_UNORM_SRGB,       {16, 4,  4, 1} },
		{ TextureFormat::BC4_UNORM,            {8,  4,  4, 1} },
		{ TextureFormat::BC4_SNORM,            {8,  4,  4, 1} },
		{ TextureFormat::BC5_UNORM,            {16, 4,  4, 1} },
		{ TextureFormat::BC5_SNORM,            {16, 4,  4, 1} },
		{ TextureFormat::BC6H_SF16,            {16, 4,  4, 1} },
		{ TextureFormat::BC6H_UF16,            {16, 4,  4, 1} },
		{ TextureFormat::BC7_UNORM,            {16, 4,  4, 1} },
		{ TextureFormat::BC7_UNORM_SRGB,       {16, 4,  4, 1} },
		{ TextureFormat::ASTC_4x4_UNORM,       {16, 4,  4, 1} },
		{ TextureFormat::ASTC_5x5_UNORM,       {16, 5,  5, 1} },
		{ TextureFormat::ASTC_8x8_UNORM,       {16, 8,  8, 1} }
	}};

	// these could be methods in the class honestly...
	
	int32 GetBpp(TegraTexture* Texture) {
		constexpr auto map = CompileTimeMap<TextureFormat, TexFormatInfo, SupportedFormatData.size()>{{SupportedFormatData}};
		auto format = map.at(Texture->Format);

		return format.bpp;
	}

	int32 GetBlockWidth(TegraTexture* Texture) {
		constexpr auto map = CompileTimeMap<TextureFormat, TexFormatInfo, SupportedFormatData.size()>{{SupportedFormatData}};
		auto format = map.at(Texture->Format);

		return format.blockwidth;
	}

	
	int32 GetBlockHeight(TegraTexture* Texture) {
		constexpr auto map = CompileTimeMap<TextureFormat, TexFormatInfo, SupportedFormatData.size()>{{SupportedFormatData}};
		auto format = map.at(Texture->Format);

		return format.blockheight;
	}


	int32 GetBlockDepth(TegraTexture* Texture) {
		constexpr auto map = CompileTimeMap<TextureFormat, TexFormatInfo, SupportedFormatData.size()>{{SupportedFormatData}};
		auto format = map.at(Texture->Format);

		return format.blockdepth;
	}

	// Ported from SwitchToolbox
	
	inline uint32 DivRoundup(uint32 n, uint32 d) { 
		return (n + d - 1) / d; 
	}

	inline uint32 roundup(uint32 x, uint32 y) { 
		return ((x - 1) | (y - 1)) + 1; 	
	}

	inline uint32 pow2_roundup(uint32 x) {
		x -= 1;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}
	

	// Really get block height.
	uint32 GetRealBlockHeight(uint32 height) {
		auto bh = pow2_roundup(height / 8);
		if (bh > 16)
			return 16;

		return bh;
	}


	/**
	 * TODO.
	 */
	uint32 GetLinearAddressBlock(uint32 x, uint32 y, uint32 width) {
		return 0;
	}

	bool TegraTexture::Deswizzle() {

		int32 bpp = GetBpp(this);
		int32 blockWidth = GetBlockWidth(this);
		int32 blockHeight = GetBlockHeight(this);
		int32 blockDepth = GetBlockDepth(this);

		// format is probably unsupported.
		if(bpp == -1)
			return false;




		return true;
	}

}
}