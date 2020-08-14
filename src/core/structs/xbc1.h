/**
 * \file 
 * XBC1 structures.
 */
#pragma once
#include <core.h>

namespace xb2at {
namespace core {

	/**
	 * XBC1 structures.
	 */
namespace xbc1 {

	
	/**
	 * XBC1 header.
	 *
	 * XBC1 files are Zlib compressed.
	 */
	struct header {
		
		/**
		 * Magic value. Should be "xbc1".
		 */
		char magic[4];

		int32 version;
		
		/**
		 * The true size of the file.
		 */
		int32 decompressedSize;
		
		/**
		 * The size of the file inside the XBC1 container.
		 */
		int32 compressedSize;


		int32 unknown1;
	};

	/**
	 * XBC1 data.
	 */
	struct xbc1 : public header {
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
		std::vector<char> data;

		/**
		 * The offset of where the compressed data is.
		 */
		int32 offset;
	};

	static_assert(sizeof(header) == 20, "Invalid XBC1 header size!");
}


}
}