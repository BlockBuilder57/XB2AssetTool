#pragma once

#include <xb2at/core/FourCC.h>
#include <xb2at/core/Stream.h>

namespace xb2at {

	namespace core {

		namespace msmd {

			/**
			 * MSMD magic
			 */
			constexpr static std::uint32_t magic = FourCCValue("MSMD");

			// TODO is there a table length anywhere????

			/**
			 * Very simplistic MSMD header
			 */
			struct header {
				std::uint32_t magic;

				std::uint8_t unknown_padding[0x5C]; // unknown padding data, this will be figured out later

				std::uint32_t other_table_offset; // could be abused to calculate the length of the xbc1 table
										// alternatively this could be the end offset of the table

				std::uint8_t unknown_padding2[0xF]; // unknown padding data, this will be figured out later

				std::uint32_t first_table_begin_offset;

				std::uint32_t dummy; // padding always 0x0

				std::uint32_t table_offset; // this is duplicated at C8?
				// in ma02a 0x70 has what seems to be the true offset
				// which in ma01a is the exact same as what's in 0x78 and c8 there


				template<core::Stream Stream>
				inline bool Transform(Stream& stream) {
					XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(magic));
					if constexpr(Stream::IsReadStream()) {
						if(magic != msmd::magic)
							return false;
					}
					XB2AT_TRANSFORM_CATCH(stream.template FixedBytes(unknown_padding));
					XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(other_table_offset));
					XB2AT_TRANSFORM_CATCH(stream.template FixedBytes(unknown_padding2));
					XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(first_table_begin_offset));
					XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(dummy));
					XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(table_offset));
					return true;
				}
			};

			/**
			 * msmd xbc1 table entry
			 */
			struct table_entry {
				/**
				 * Offset in wismda file where xbc1 is.
				 */
				std::uint32_t offset;
				
				/**
				 * Matches exactly with the decompressed size in the XBC1 header.
				 * Unknown why this value is repeated here, maybe to avoid seeking into the wismda?
				 */
				std::uint32_t decompressed_size;

				template<core::Stream Stream>
				inline bool Transform(Stream& stream) {
					XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(offset));
					XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(decompressed_size));
					return true;
				}
			};
		
		}

	}
} // namespace xb2at