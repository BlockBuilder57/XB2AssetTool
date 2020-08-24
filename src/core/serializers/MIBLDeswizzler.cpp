#include <core.h>
#include <array>
#include <serializers/MIBLDeswizzler.h>

#include <lowlevelmath.h>

namespace xb2at {
	namespace core {

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
					logger.warn("This format is a bit buggy for the time being..");
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
					logger.error("Unknown/Unhandled MIBL type ", (int)texture.type, "!");
					break;
			}
		}

		void MIBLDeswizzler::Write(fs::path& path) {
			std::ofstream stream(path.string(), std::ofstream::binary);

			DdsHeader header;

			// Setup the header by clearing a few things
			memset(&header.reserved, 0, sizeof(header.reserved) / sizeof(uint32));

			header.height = texture.height;
			header.width = texture.width;
			header.pitchOrLinearSize = texture.data.size();

			// Setup the pixel format
			switch(Format) {
				// In this case we need to actually fill out the pixel format structure.
				// This isn't terribly hard, thankfully.
				case TextureFormat::R8G8B8A8_UNORM:
					header.pixFormat.flags = 0x00000041; // DDS_RGBA (DDSPF_RGB | ALPHAPIXELS)
					header.pixFormat.fourcc = 0;

					header.pixFormat.RgbBitCount = 32;

					header.pixFormat.RbitMask = 0x000000ff;
					header.pixFormat.GbitMask = 0x0000ff00;
					header.pixFormat.BbitMask = 0x00ff0000;
					header.pixFormat.AbitMask = 0xff000000;
					break;

				case TextureFormat::BC1_UNORM:
					header.pixFormat.fourcc = 0x31545844; // DXT1
					break;

				case TextureFormat::BC2_UNORM:
					header.pixFormat.fourcc = 0x33545844; // DXT3
					break;

				case TextureFormat::BC3_UNORM:
					header.pixFormat.fourcc = 0x35545844; // DXT5
					break;

				case TextureFormat::BC4_UNORM:
					header.pixFormat.fourcc = 0x31495441; // ATI1
					break;

				case TextureFormat::BC5_UNORM:
					header.pixFormat.fourcc = 0x32495441; // ATI2
					break;

				default:
					// Default to writing the DX10 DDS magic.
					header.pixFormat.fourcc = 0x30315844;
					break;
			}

			// Write the DDS header
			stream.write((char*)&header, sizeof(DdsHeader));

			// Write the DX10 format header if required.
			if(header.pixFormat.fourcc == 0x30315844) {
				DdsHeader::Dx10Header dx10;
				dx10.format = Format;
				stream.write((char*)&dx10, sizeof(DdsHeader::Dx10Header));
			}

			stream.write(&texture.data[0], texture.data.size());
		}
	} // namespace core
} // namespace xb2at