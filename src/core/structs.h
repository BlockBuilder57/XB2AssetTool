#pragma once
#include <core.h>

namespace xb2at {
namespace core {

namespace xbc1 {

	struct xbc1_header {
		char magic[4];

		int32 version;
		int32 decompressedSize;
		int32 compressedSize;
		int32 unknown1;
	};

	struct xbc1 : public xbc1_header {
		std::string name;
		std::vector<char> data;
		int32 offset;
	};

}

namespace msrd {

		enum data_item_type : int16 {
			model = 0,
			shaderbundle,
			cachedtextures,
			texture
		};

		struct data_item {
			int32 offset;
			int32 size;
			int16 tocIndex;
			data_item_type type;
		};

		struct toc_entry {
			int32 compressedSize;
			int32 fileSize;
			int32 offset;
		};

		struct texture_info {
			int32 unknown;
			int32 size;
			int32 offset;
			int32 stringOffset;
		};

		struct texture_header {
			int32 size;
			int32 offset;
		};

		struct texture : public texture_header {
			std::string name;
		};

		struct msrd_header {
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

		struct msrd : public msrd_header {

			std::vector<data_item> dataItems;
			std::vector<toc_entry> toc;

			std::vector<xbc1::xbc1> files;

			std::vector<int16> textureIds;
			int32 textureCount;
			int32 textureChunkSize;

			int32 unknown2;
			int32 textureStringBufferOffset;

			std::vector<texture_info> textureInfo;
			std::vector<std::string> textureNames;
		};

}

}
}
