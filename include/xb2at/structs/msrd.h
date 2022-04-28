/**
 * \file 
 * MSRD structures.
 *
 */
#pragma once

#include <xb2at/core/UnderlyingValue.h>
#include <xb2at/core/Stream.h>
// TODO: can we get away with forward decls?
#include <xb2at/structs/xbc1.h>
#include <xb2at/structs/mesh.h>
#include <xb2at/structs/mibl.h>

namespace xb2at::core::msrd {

	/**
	 * The possible types of a data item.
	 */
	enum DataItemType : std::uint16_t {
		Model = 0,
		ShaderBundle,
		CachedTextures,
		Texture
	};

	/**
	 * A data item.
	 */
	struct DataItem {
		/**
		 * The offset of the data item.
		 */
		std::uint32_t offset;

		/**
		 * Size of the data item.
		 */
		std::uint32_t size;

		/**
		 * Index in the TOC.
		 */
		std::uint16_t tocIndex;

		/**
		 * Type of the data item.
		 */
		DataItemType type;

		std::uint8_t unknown1[0x8]; // possibly reserved padding data?

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(offset));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(size));
			XB2AT_TRANSFORM_CATCH(stream.template Uint16<std::endian::little>(tocIndex));
			XB2AT_TRANSFORM_CATCH(stream.template Uint16<std::endian::little>(core::UnderlyingValue(type)));
			XB2AT_TRANSFORM_CATCH(stream.template FixedSizeArray(unknown1));
			return true;
		}
	};

	/**
	 * An entry in the TOC.
	 */
	struct TocEntry {
		/**
		 * Compressed file size.
		 */
		std::uint32_t compressedSize;

		/**
		 * True/decompressed file size.
		 */
		std::uint32_t fileSize;

		/**
		 * Offset of where the file is.
		 */
		std::uint32_t offset;

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(compressedSize));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(fileSize));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(offset));
			return true;
		}
	};

	/**
	 * Texture information.
	*/
	struct TextureInfo {
		std::uint32_t unknown;

		/**
		 * Size of the texture.
		 */
		std::uint32_t size;

		/**
		 * Offset to start of texture data.
		 */
		std::uint32_t offset;

		/**
		 * Offset to position in the string buffer where this texture's name is.
		 */
		std::uint32_t stringOffset;

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(unknown));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(size));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(offset));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(stringOffset));
			return true;
		}
	};

	struct TextureHeader {
		/**
		 * Size (repeated again)
		 */
		std::uint32_t size;

		/**
		 * Offset to start of texture data (repeated again)
		 */
		std::uint32_t offset;

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(size));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(offset));
			return true;
		}
	};

	struct Texture : public TextureHeader {
		/**
		 * Name of the texture.
		 */
		std::string name;
	};

	/**
	 * MSRD header
	 */
	struct MsrdHeader {
		/**
		 * Magic value. Should be "DRSM"
		 */
		char magic[4];

		std::uint32_t version;
		std::uint32_t headerSize;
		std::uint32_t offset;

		std::uint32_t tag;
		std::uint32_t revision;

		std::uint32_t dataitemsCount;
		std::uint32_t dataitemsOffset;
		std::uint32_t fileCount;
		std::uint32_t tocOffset;

		std::uint8_t unknown1[0x1c];

		std::uint32_t textureIdsCount;
		std::uint32_t textureIdsOffset;
		std::uint32_t textureCountOffset;

		template<core::Stream Stream>
		inline bool Transform(Stream& stream) {
			XB2AT_TRANSFORM_CATCH(stream.template FixedString(magic)); //TODO: FourCC

			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(version));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(headerSize));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(offset));

			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(tag));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(revision));

			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(dataitemsCount));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(dataitemsOffset));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(fileCount));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(tocOffset));

			XB2AT_TRANSFORM_CATCH(stream.template FixedSizeArray(unknown1));

			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(textureIdsCount));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(textureIdsOffset));
			XB2AT_TRANSFORM_CATCH(stream.template Uint32<std::endian::little>(textureCountOffset));
			return true;
		}
	};

	/**
	 * MSRD data
	 */
	struct Msrd {
		MsrdHeader header;

		std::vector<DataItem> dataItems;
		std::vector<TocEntry> toc;

		/**
		 * XBC1 files
		 */
		std::vector<Xbc1> files;

		std::vector<mesh::mesh> meshes;

		std::vector<std::uint16_t> textureIds;
		std::uint32_t textureCount;
		std::uint32_t textureChunkSize;

		std::uint32_t unknown2;
		std::uint32_t textureStringBufferOffset;

		std::vector<TextureInfo> textureInfo;
		std::vector<std::string> textureNames;

		/**
		 * List of textures
		 */
		std::vector<mibl::texture> textures;
	};

} // namespace xb2at::core::msrd