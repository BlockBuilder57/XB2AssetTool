/**
 * \file 
 * XBC1 structures.
 */
#pragma once
#include <string>
#include <vector>

#include <xb2at/core/FourCC.h>
#include <xb2at/core/Stream.h>

namespace xb2at::core {

	/**
	 * XBC1 data.
	 */
	struct Xbc1 {

		/**
		 * XBC1 magic
		 */
		constexpr static std::uint32_t magic = FourCCValue("xbc1");

		/**
	 	 * XBC1 header.
	 	 *
	 	 * XBC1 files are Zlib compressed.
	 	 */
		struct Header {
			/**
			 * Magic value. Should be "xbc1".
			 */
			std::uint32_t magic;

			std::int32_t version;

			/**
			 * The true size of the file.
			 */
			std::int32_t decompressedSize;

			/**
			 * The size of the file inside the XBC1 container.
			 */
			std::int32_t compressedSize;

			std::int32_t unknown1;

			template<core::Stream Stream>
			inline bool Transform(Stream& stream) {
				XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(magic));
				XB2AT_TRANSFORM_CATCH(stream.template Int32<std::endian::little>(version));
				XB2AT_TRANSFORM_CATCH(stream.template Int32<std::endian::little>(decompressedSize));
				XB2AT_TRANSFORM_CATCH(stream.template Int32<std::endian::little>(compressedSize));
				XB2AT_TRANSFORM_CATCH(stream.template Int32<std::endian::little>(unknown1));
				return true;
			}
		};

		static_assert(sizeof(Header) == 20, "[xb2at::core::Xbc1::Header] Invalid XBC1 header size!");

		Header header{};

		/**
		 * The name the packing tool decided for this XBC1.
		 * This can either be something that makes *some* sense or no sense at all
		 * depending on what day of the week it was
		 * and the lunar alignments of the planets at the specific time the file was packed.
		 */
		std::string name;

		/**
		 * The decompressed data of the file.
		 */
		std::vector<std::uint8_t> data;

		/**
		 * The offset of where the compressed data is.
		 */
		std::uint32_t offset;
	};
} // namespace xb2at::core