#pragma once
#include <xb2at/core.h>

namespace xb2at {

	namespace core {

		namespace msmd {

			/**
			 * MSMD magic
			 */
			constexpr uint32 magic = MakeFourCC("MSMD");

			// TODO is there a table length anywhere????

			/**
			 * Very simplistic MSMD header
			 */
			struct header {
				uint32 magic;

				byte unknown_padding[0x5C]; // unknown padding data, this will be figured out later

				uint32 other_table_offset; // could be abused to calculate the length of the xbc1 table
										// alternatively this could be the end offset of the table

				byte unknown_padding2[0xF]; // unknown padding data, this will be figured out later

				uint32 first_table_begin_offset;

				uint32 dummy; // padding always 0x0

				uint32 table_offset; // this is duplicated at C8? 
				// in ma02a 0x70 has what seems to be the true offset
				// which in ma01a is the exact same as what's in 0x78 and c8 there
			};

			/**
			 * msmd xbc1 table entry
			 */
			struct table_entry {
				uint32 offset;
				uint32 size;
			};
		
		}

	}
} // namespace xb2at