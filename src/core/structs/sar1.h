/**
 * \file 
 * SAR1 structures.
 */
#pragma once
#include <core.h>

namespace xb2at {
namespace core {

	/**
	 * SAR1 structures.
	 */
namespace sar1 {

	struct bc_data {
		char magic[4];

		int32 blockCount;
		int32 fileSize;
		int32 pointerCount;
		int32 offsetToData;
		char unknown1[0x10];
	};

	struct bc : public bc_data {
		std::vector<char> data;
	};

	struct toc_data {
		int32 offset;
		int32 size;
		int32 unknown1;
	};

	struct toc : public toc_data {
		std::string filename;
	};

	/**
	 * SAR1 header.
	 */
	struct header {

		/**
		 * Magic value. Should be "1RAS".
		 */
		char magic[4];
		
		int32 fileSize;
		int32 version;
		int32 numFiles;
		int32 tocOffset;
		int32 dataOffset;
		int32 unknown1;
		int32 unknown2;

	};

	/**
	 * SAR1 data.
	 */
	struct sar1 : public header {
		
		std::string path; //0x80 of space

		std::vector<toc> tocItems;
		std::vector<bc> bcItems;

	};
}


}
}