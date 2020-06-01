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
		 * LBIM file header.
		 */
		struct header {
			uint32 magic; // MIBL

			int32 unknown5;
			int32 unknown4;

			int32 width;
			int32 height;

			int32 unknown3;
			int32 unknown2;

			int32 type;
			int32 unknown1;
			int32 version;
		};

		/**
		 * LBIM texture
		 */
		struct texture : public header {
			std::string filename;
			std::vector<char> data;
		};

		/**
		 * typedef for easier usage
		 */
		typedef std::vector<texture> texturelist;
	}
}
}