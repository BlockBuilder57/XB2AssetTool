/**
 * \file
 * MIBL (Multi IBL Texture) structures
 */
#pragma once
#include <xb2at/core/FourCC.h>
#include <xb2at/core/UnderlyingValue.h>
#include <xb2at/core/Stream.h>

// only need a forward decl of this
namespace xb2at::core::xbc1 { struct xbc1; };

namespace xb2at::core::mibl {

	/**
	 * MIBL texture format
	 */
	enum class MiblTextureFormat : std::uint32_t {
		R8G8B8A8_UNORM = 37,

		/**
		 * DXT1
		 */
		BC1_UNORM = 66,
		BC3_UNORM = 68,
		BC4_UNORM = 73,
		BC5_UNORM = 75,
		BC7_UNORM = 77
	};

	/**
	 * MIBL magic value.
	 */
	constexpr std::uint32_t magic = core::FourCCValue("MIBL");

	/**
	 * MIBL file header.
	 * This is intentionally stored backwards
	 * due to the fact we one-shot read for optimization purposes.
	 */
	struct header {
		/**
		 * Size of the texture. Seems to line up with MSRD dataitem size.
		 */
		std::uint32_t dataSize;

		/**
		 * Size of the header. Always 4096.
		 */
		std::uint32_t headerSize;

		/**
		 * The width of the texture.
		 */
		std::uint32_t width;

		/**
		 * The height of the texture.
		 */
		std::uint32_t height;

		/**
		 * Depth of texture in texels.
		 */
		std::uint32_t depth;

		/**
		 * Texture target in NVN format.
		 */
		std::uint32_t textureTarget;

		/**
		 * Texture format in NVN format.
		 */
		MiblTextureFormat type;

		/**
		 * Amount of mips in the buffer.
		 */
		std::uint32_t mipLevels;

		/**
		 * The version value.
		 * Typically 0x2711 (10001).
		 */
		std::uint32_t version;

		/**
		 * The magic value.
		 * *ALWAYS* 0x4D49424C.
		 */
		std::uint32_t magic;

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(dataSize));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(headerSize));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(width));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(height));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(textureTarget));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(core::UnderlyingValue(type)));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(mipLevels));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(version));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(magic));
			return true;
		}
	};

	/**
	 * Wrapper over the MIBL texture header
	 * allowing us to store data and such
	 */
	struct texture {
		header header;

		//std::uint32_t offset;
		std::uint32_t size;

		std::string filename;
		std::vector<std::uint8_t> data;

		/**
		 * True if the texture is a CachedTexture.
		 */
		bool cached;

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			XB2AT_TRANSFORM_CATCH(header.template Transform(stream));
			return true;
		}
	};

} // namespace xb2at::core::mibl