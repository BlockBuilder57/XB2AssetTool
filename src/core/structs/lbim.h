/**
 * \file
 * LBIM (texture) structures.
 */
#pragma once
#include <core.h>

namespace xb2at {
namespace core {

	// NOTE(lily): RE of XBDE has possibly confirmed the name 
	// is not "LBIM" like ordered here, but MIBL (Multi IBL?)
	// possible name changes here that may break code

	/**
	 * LBIM (texture) structures
	 */
	namespace lbim {

		/**
		 * LBIM texture format
		 */
		enum class LbimTextureFormat : int32 {
			R8G8B8A8_UNORM = 37,
			
			/**
			 * DXT1
			 */
			BC1_UNORM = 66,
			BC3_UNORM = 68,
			BC4_UNORM = 73,
			BC5_UNORM = 75
		};

		/**
		 * LBIM magic value.
		 */
		constexpr int32 magic = 0x4D49424C;

		/**
		 * LBIM file header.
		 * This is intentionally stored backwards
		 * due to the fact we one-shot read for optimization purposes.
		 */
		struct header {
			int32 unknown4; // this is known as "datasize" in xenolib code, may rename
			int32 unknown3; // this is known as "headersize" in xenolib code

			/**
			 * The width of the texture. 
			 */
			int32 width;

			/**
			 * The height of the texture. 
			 */
			int32 height;
			
			int32 unknown2;
			int32 unknown1;

			/**
			 * texture format in NVN format
			 */
			LbimTextureFormat type;

			int32 unknown0;

			/**
			 * The version value.
			 * Typically 0x2711 (10001).
			 */
			int32 version;
			
			/**
			 * The magic value.
			 * *ALWAYS* 0x4D49424C
			 */
			int32 magic;
		};

		/**
		 * Wrapper over the LBIM texture header
		 * allowing us to store data and such
		 */
		struct texture : public header {

			int32 offset;
			int32 size;

			std::string filename;
			std::vector<char> data;



			/**
			 * True if the texture is a CachedTexture. 
			 */
			bool cached;
		};

	}
}
}