/**
 * \file 
 * MSRD structures.
 *
 */
#pragma once
#include <core.h>
#include "xbc1.h"
#include "mesh.h"
#include "lbim.h"

namespace xb2at {
	namespace core {

		/**
		 * MSRD structures.
		 */
		namespace msrd {

			/**
			 * The possible types of a data item.
			 */
			enum data_item_type : int16 {
				Model = 0,
				ShaderBundle,
				CachedTextures,
				Texture
			};

			/**
			 * A data item.
			 */
			struct data_item {
				/**
				 * The offset of the data item.
				 */
				int32 offset;

				/**
				 * Size of the data item.
				 */
				int32 size;

				/**
				 * Index in the TOC.
				 */
				int16 tocIndex;

				/**
				 * Type of the data item.
				 */
				data_item_type type;
			};

			/**
			 * A entry in the TOC.
			 */
			struct toc_entry {
				/**
				 * Compressed file size.
				 */
				int32 compressedSize;

				/**
				 * True/decompressed file size.
				 */
				int32 fileSize;

				/**
				 * Offset of where the file is.
				 */
				int32 offset;
			};

			/**
			 * Texture information.
			*/
			struct texture_info {
				int32 unknown;

				/**
				 * Size of the texture.
				 */
				int32 size;

				/**
				 * Offset to start of texture data.
				 */
				int32 offset;

				/**
				 * Offset to position in the string buffer where this texture's name is.
				 */
				int32 stringOffset;
			};

			struct texture_header {
				/**
				 * Size (repeated again)
				 */
				int32 size;

				/**
				 * Offset to start of texture data (repeated again)
				 */
				int32 offset;
			};

			struct texture : public texture_header {
				/**
				 * Name of the texture.
				 */
				std::string name;
			};

			/**
			 * MSRD header
			 */
			struct msrd_header {
				/**
				 * Magic value. Should be "DRSM"
				 */
				char magic[4];

				int32 version;
				int32 headerSize;
				int32 offset;

				int32 tag;
				int32 revision;

				int32 dataitemsCount;
				int32 dataitemsOffset;
				int32 fileCount;
				int32 tocOffset;

				byte unknown1[0x1c];

				int32 textureIdsCount;
				int32 textureIdsOffset;
				int32 textureCountOffset;
			};

			/**
			 * MSRD data
			 */
			struct msrd : public msrd_header {
				std::vector<data_item> dataItems;
				std::vector<toc_entry> toc;

				/**
				 * XBC1 files
				 */
				std::vector<xbc1::xbc1> files;

				std::vector<mesh::mesh> meshes;

				std::vector<int16> textureIds;
				int32 textureCount;
				int32 textureChunkSize;

				int32 unknown2;
				int32 textureStringBufferOffset;

				std::vector<texture_info> textureInfo;
				std::vector<std::string> textureNames;

				/**
				 * List of textures
				 */
				std::vector<mibl::texture> textures;
			};

		} // namespace msrd

	} // namespace core
} // namespace xb2at