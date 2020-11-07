#pragma once
#include <core.h>

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
				byte unknown_padding[0x77]; // unknown padding data
				uint32 table_offset;
			};

			struct table_entry {
				uint32 offset;
				uint32 size;
			};
		
		}

	}
} // namespace xb2at