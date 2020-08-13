/**
 * \file
 * LBIM (texture) structures.
 */
#pragma once
#include <core.h>

namespace xb2at {
namespace core {

	/**
	 * LBIM (texture) structures
	 */
	namespace lbim {

		/**
		 * LBIM magic value.
		 */
		constexpr int32 magic = 0x4D49424C;

		/**
		 * LBIM file header.
		 */
		struct header {
			int32 unknown4; // "datasize" in xenolib
			int32 unknown3; // "headersize" in xenolib

			int32 width;
			int32 height;
			
			int32 unknown2;
			int32 unknown1;

			int32 type;
			int32 unknown0;
			int32 version;
			
			int32 magic;
		};

		/**
		 * LBIM texture
		 */
		struct texture : public header {

			int32 offset;

			std::string filename;
			std::vector<char> data;
		};

	}
}
}