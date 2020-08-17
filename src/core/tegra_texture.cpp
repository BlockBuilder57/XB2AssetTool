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
		uint32 bitsPerPixel;
		
		uint32 blockwidth;

		uint32 blockheight;

	};

	/**
	 * Constexpr array of all pairs of supported format in the layout of
	 *	{ ID, { INFO } }
	 * 
	 * The idea is if this table ends up going in the binary it will be fairly packed since it's an array of pairs
	 * which means it will be smaller then the map repressentation, and then we can use
	 * our constexpr map for smaller code size while still maintaining a performance increase
	 * (because the search will be most likely faster since this is a small dataset.)
	 */
	constexpr static std::array<std::pair<TextureFormat, TexFormatInfo>, 30> SupportedFormatData {{
		// format id, bpp, block width, block height, block depth
		{ TextureFormat::R8_UNORM,             {1,  1,  1} },
		{ TextureFormat::R5G5B5A1_UNORM,       {2,  1,  1} },
		{ TextureFormat::B5G6R5_UNORM,         {2,  1,  1} },
		{ TextureFormat::R16_UNORM,            {2,  1,  1} },
		{ TextureFormat::R8G8B8A8_UNORM,       {4,  1,  1} },
		{ TextureFormat::R8G8B8A8_UNORM_SRGB,  {4,  1,  1} },
		{ TextureFormat::R8G8B8A8_UNORM_SRGB,  {4,  1,  1} },
		{ TextureFormat::R11G11B10_FLOAT,      {4,  1,  1} },
		{ TextureFormat::R32_FLOAT,            {4,  1,  1} },
		{ TextureFormat::D32_FLOAT_S8X24_UINT, {8,  1,  1} },
		{ TextureFormat::BC1_UNORM,            {8,  4,  4} },
		{ TextureFormat::BC1_UNORM_SRGB,       {8,  4,  4} },
		{ TextureFormat::BC1_UNORM_SRGB,       {8,  4,  4} },
		{ TextureFormat::BC2_UNORM,            {16, 4,  4} },
		{ TextureFormat::BC2_UNORM_SRGB,       {16, 4,  4} },
		{ TextureFormat::BC2_UNORM_SRGB,       {16, 4,  4} },
		{ TextureFormat::BC3_UNORM,            {16, 4,  4} },
		{ TextureFormat::BC3_UNORM_SRGB,       {16, 4,  4} },
		{ TextureFormat::BC3_UNORM_SRGB,       {16, 4,  4} },
		{ TextureFormat::BC4_UNORM,            {8,  4,  4} },
		{ TextureFormat::BC4_SNORM,            {8,  4,  4} },
		{ TextureFormat::BC5_UNORM,            {16, 4,  4} },
		{ TextureFormat::BC5_SNORM,            {16, 4,  4} },
		{ TextureFormat::BC6H_SF16,            {16, 4,  4} },
		{ TextureFormat::BC6H_UF16,            {16, 4,  4} },
		{ TextureFormat::BC7_UNORM,            {16, 4,  4} },
		{ TextureFormat::BC7_UNORM_SRGB,       {16, 4,  4} },
		{ TextureFormat::ASTC_4x4_UNORM,       {16, 4,  4} },
		{ TextureFormat::ASTC_5x5_UNORM,       {16, 5,  5} },
		{ TextureFormat::ASTC_8x8_UNORM,       {16, 8,  8} }
	}};

	// If you want a idea of how these functions work you can look at the CompileTimeMap template

	uint32 TegraX1SwizzledTexture::GetBpp() {
		constexpr static auto map = CompileTimeMap<TextureFormat, TexFormatInfo, SupportedFormatData.size()>{{SupportedFormatData}};
		auto format = map.at(Format);

		return format.bitsPerPixel;
	}

	uint32 TegraX1SwizzledTexture::GetBlockWidth() {
		constexpr static auto map = CompileTimeMap<TextureFormat, TexFormatInfo, SupportedFormatData.size()>{{SupportedFormatData}};
		auto format = map.at(Format);

		return format.blockwidth;
	}

	uint32 TegraX1SwizzledTexture::GetBlockHeight() {
		constexpr static auto map = CompileTimeMap<TextureFormat, TexFormatInfo, SupportedFormatData.size()>{{SupportedFormatData}};
		auto format = map.at(Format);

		return format.blockheight;
	}

	// Ported from SwitchToolbox
	
	inline uint32 DivRoundUp(uint32 n, uint32 d) { 
		return (n + d - 1) / d; 
	}

	inline uint32 RoundUp(uint32 x, uint32 y) { 
		return ((x - 1) | (y - 1)) + 1; 	
	}

	inline uint32 Pow2RoundUp(uint32 x) {
		x -= 1;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x + 1;
	}
	

	uint32 GetRealBlockHeight(uint32 height) {
		auto bh = Pow2RoundUp(height / 8);
		if (bh > 16)
			return 16;

		return bh;
	}

	/**
	 * Get linear address block from given data.
	 * From Tegra X1 TRM.
	 */
	uint32 GetLinearAddressBlock(uint32 x, uint32 y, uint32 width, uint32 bpp, uint32 base, uint32 blockHeight) {
		uint32 gobWidth = DivRoundUp(width * bpp, 64);
		uint32 gobAddress = (base + (y / (8 * blockHeight)) * 512 * blockHeight * gobWidth
								+ (x * bpp / 64) * 512 * blockHeight
								+ (y % (8 * blockHeight) / 8) * 512);

		x *= bpp;

		return (gobAddress + ((x % 64) / 32) * 256 + ((y % 8) / 2) * 64
				+ ((x % 32) / 16) * 32 + (y % 2) * 16 + (x % 16));

	}

	/**
	 * Struct used for better argument passing to the deswizzle function.
	 */
	struct SwizzleData {
		uint32 width; 
		uint32 height; 
		uint32 bpp;

		uint32 blockWidth;
		uint32 blockHeight32;
		uint32 roundPitch;
		uint32 blockHeightLog2;

		/**
		 * linear idk id this is even used 
		 */
		bytebool linear;
	};
	
	void DeSwizzle(const SwizzleData& swizzledata, std::vector<byte>& data) {
		// NOTE: The block depth for every format we care about is one,
		// so this is perfectly safe to assume.
		constexpr uint32 blockDepth = 1;
	}


	void TegraX1SwizzledTexture::Deswizzle() {

		// Create our swizzle data structure
		// that we'll pass to the internal deswiz function first.
		SwizzleData info { 
			width,
			height, 
			GetBpp(),
			GetBlockWidth(),
			GetRealBlockHeight(GetBlockHeight())
			-1 /* block height log2, i'll have to see how to do this right (AKA. NOT WITH A STUPID HACK :VVVVVVVVVVVVVVVVVV) */
		};

		// Create a copy of the texture data that we will work with.
		// Once we are done, we will move this data *back* to the main texture data.
		std::vector<byte> dataCopy = std::vector<byte>(Data.begin(), Data.end());

		DeSwizzle(info, dataCopy);

		// if it worked, then we will move the data copy back..

		// mark that we were able to deswizzle
		deswizzled = true;
	}

}
}