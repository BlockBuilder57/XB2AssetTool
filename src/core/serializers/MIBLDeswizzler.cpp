#include <core.h>
#include <array>
#include <serializers/MIBLDeswizzler.h>

#include <lowlevelmath.h>

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
		 * The idea is if this table ends up going in the binary it will be fairly packed
		 * since it's an array of pairs (which will get compressed down to just T1,T2 *in* the array),
		 * meaning it will be smaller than the map repressentation (especially since none of the map code will exist), 
		 * and then we can use our constexpr map for smaller code size 
		 * while still maintaining a performance increase.
		 */
		// clang-format off

		constexpr static std::array<std::pair<TextureFormat, TexFormatInfo>, 30> SupportedFormatData {{ 
				// Format ID                           bpp  bw  bh
				{ TextureFormat::R8_UNORM, { 1, 1, 1 } },
				{ TextureFormat::R5G5B5A1_UNORM, { 2, 1, 1 } },
				{ TextureFormat::B5G6R5_UNORM, { 2, 1, 1 } },
				{ TextureFormat::R16_UNORM, { 2, 1, 1 } },
				{ TextureFormat::R8G8B8A8_UNORM, { 4, 1, 1 } },
				{ TextureFormat::R8G8B8A8_UNORM_SRGB, { 4, 1, 1 } },
				{ TextureFormat::R8G8B8A8_UNORM_SRGB, { 4, 1, 1 } },
				{ TextureFormat::R11G11B10_FLOAT, { 4, 1, 1 } },
				{ TextureFormat::R32_FLOAT, { 4, 1, 1 } },
				{ TextureFormat::D32_FLOAT_S8X24_UINT, { 8, 1, 1 } },
				{ TextureFormat::BC1_UNORM, { 8, 4, 4 } },
				{ TextureFormat::BC1_UNORM_SRGB, { 8, 4, 4 } },
				{ TextureFormat::BC1_UNORM_SRGB, { 8, 4, 4 } },
				{ TextureFormat::BC2_UNORM, { 16, 4, 4 } },
				{ TextureFormat::BC2_UNORM_SRGB, { 16, 4, 4 } },
				{ TextureFormat::BC2_UNORM_SRGB, { 16, 4, 4 } },
				{ TextureFormat::BC3_UNORM, { 16, 4, 4 } },
				{ TextureFormat::BC3_UNORM_SRGB, { 16, 4, 4 } },
				{ TextureFormat::BC3_UNORM_SRGB, { 16, 4, 4 } },
				{ TextureFormat::BC4_UNORM, { 8, 4, 4 } },
				{ TextureFormat::BC4_SNORM, { 8, 4, 4 } },
				{ TextureFormat::BC5_UNORM, { 16, 4, 4 } },
				{ TextureFormat::BC5_SNORM, { 16, 4, 4 } },
				{ TextureFormat::BC6H_SF16, { 16, 4, 4 } },
				{ TextureFormat::BC6H_UF16, { 16, 4, 4 } },
				{ TextureFormat::BC7_UNORM, { 16, 4, 4 } },
				{ TextureFormat::BC7_UNORM_SRGB, { 16, 4, 4 } },
				{ TextureFormat::ASTC_4x4_UNORM, { 16, 4, 4 } },
				{ TextureFormat::ASTC_5x5_UNORM, { 16, 5, 5 } },
				{ TextureFormat::ASTC_8x8_UNORM, { 16, 8, 8 } }
		}};

		// clang-format on

		// If you want a idea of how these functions work you can look at the CompileTimeMap template

		uint32 MIBLDeswizzler::GetBpp() {
			constexpr static auto map = CompileTimeMap<TextureFormat, TexFormatInfo, SupportedFormatData.size()> { { SupportedFormatData } };
			auto format = map.at(Format);

			return format.bitsPerPixel;
		}

		uint32 MIBLDeswizzler::GetBlockWidth() {
			constexpr static auto map = CompileTimeMap<TextureFormat, TexFormatInfo, SupportedFormatData.size()> { { SupportedFormatData } };
			auto format = map.at(Format);

			return format.blockwidth;
		}

		uint32 MIBLDeswizzler::GetBlockHeight() {
			constexpr static auto map = CompileTimeMap<TextureFormat, TexFormatInfo, SupportedFormatData.size()> { { SupportedFormatData } };
			auto format = map.at(Format);

			return format.blockheight;
		}

		constexpr bool IsPow2(int value) {
			return (value & (value - 1)) == 0;
		}

		constexpr int RoundSize(int size, int pad) {
			int mask = pad - 1;

			if((size & mask) != 0) {
				size &= ~mask;
				size += pad;
			}

			return size;
		}

		constexpr std::uint32_t Pow2RoundUp(std::uint32_t x) {
			x--;

			x |= x >> 1;
			x |= x >> 2;
			x |= x >> 4;
			x |= x >> 8;
			x |= x >> 16;
			return x + 1;
		}

		/**
		 * Get linear address block from given data.
		 */
		int GetAddr(int x, int y, int xb, int yb, int width, int xBase) {
			int xCnt = xBase;
			int yCnt = 1;
			int xUsed = 0;
			int yUsed = 0;
			int address = 0;

			while(xUsed < xBase + 2 && xUsed + xCnt < xb) {
				int xMask = (1 << xCnt) - 1;
				int yMask = (1 << yCnt) - 1;

				address |= (x & xMask) << (xUsed + yUsed);
				address |= (y & yMask) << (xUsed + (yUsed + xCnt));

				x >>= xCnt;
				y >>= yCnt;

				xUsed += xCnt;
				yUsed += yCnt;

				xCnt = std::max(std::min(xb - xUsed, 1), 0);
				yCnt = std::max(std::min(yb - yUsed, yCnt << 1), 0);
			}

			address |= (x + y * (width >> xUsed)) << (xUsed + yUsed);
			return address;
		}

		/**
		 * Internal function that actually performs deswizzle.
		 * 
		 * \param[in] texture Texture instance.
		 * \param[in] bppPower r
		 * \param[in] logger Logger instance
		 */
		void MIBLDeswizzler::SwizzleInternal(int bppPower, int swizzleSize) {

			const int bpp = 1 << bppPower;

			const int len = texture.data.size();

			const int originWidth = (texture.width + 3) / 4;
			const int originHeight = (texture.height + 3) / 4;

			const int xb = count_zeros(Pow2RoundUp(originWidth));

			const int hh = Pow2RoundUp(originHeight) >> 1;

			// TODO(lily): figure out some way to make this const
			// there's ZERO reason it needs to be mutable at any point
			// and it could easily be made immutable

			int yb = count_zeros(Pow2RoundUp(originHeight));

			if(!IsPow2(originHeight) && originHeight <= hh + hh / 3 && yb > 3)
				--yb;

			auto result = std::vector<char>(len);

			const int width = RoundSize(originWidth, 64 >> bppPower);
			const int xBase = 4 - bppPower;

			int posOut = 0;

			for(int y = 0; y < originHeight; y++) {
				for(int x = 0; x < originWidth; x++) {
					const int pos = GetAddr(x, y, xb, yb, width, xBase) * bpp;

					// then deswizzle by memcpying the bpp block out
					if(posOut + bpp <= len && pos + bpp <= len)
						memcpy(&result[posOut], &texture.data[pos], bpp);

					posOut += bpp;
				}
			}

			texture.data = result;
		}

		void MIBLDeswizzler::Deswizzle() {
			// Call the deswizzle internal routine
			switch(texture.type) {
				case mibl::MiblTextureFormat::R8G8B8A8_UNORM:
					// Not quite sure how to deal with this so..
					logger.warn("Not handled yet, but NOTE: this is DE's most common format.");
					SwizzleInternal(4, 1);
					break;
				case mibl::MiblTextureFormat::BC1_UNORM:
					SwizzleInternal(3);
					break;
				case mibl::MiblTextureFormat::BC3_UNORM:
					SwizzleInternal(4);
					break;
				case mibl::MiblTextureFormat::BC4_UNORM:
					SwizzleInternal(3);
					break;
				case mibl::MiblTextureFormat::BC5_UNORM:
					SwizzleInternal(4);
					break;
				case mibl::MiblTextureFormat::BC7_UNORM:
					SwizzleInternal(4);
					break;

					// and then mibl types not handled Go Here
				default:
					logger.error("Unknown/Unhandled LBIM type ", (int)texture.type, "!");
					break;
			}
		}

	} // namespace core
} // namespace xb2at