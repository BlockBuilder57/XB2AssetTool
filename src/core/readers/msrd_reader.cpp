#include "msrd_reader.h"
#include "streamhelper.h"
#include "endian_swap.h"
#include "xbc1_reader.h"
#include "mesh_reader.h"

namespace xb2at {
namespace core {

	msrd::msrd msrdReader::Read(const msrdReaderOptions& opts) {
		StreamHelper reader(stream);
		msrd::msrd data;

		// Read the initial header
		if(!reader.ReadType<msrd::msrd_header>(data)) {
			CheckedProgressUpdate("file could not be read", ProgressType::Error);
			return data;
		}

		if(!strncmp(data.magic, "DRSM", sizeof("DRSM"))) {
				CheckedProgressUpdate("file is not MSRD", ProgressType::Error);
			return data;
		}

		PROGRESS_UPDATE(ProgressType::Verbose, "MSRD version: " << data.version << " (0x" << std::hex << data.version << ")")
		
		if(data.dataitemsOffset != 0) {
			stream.seekg(data.offset + data.dataitemsOffset, std::istream::beg);
			data.dataItems.resize(data.dataitemsCount);

			for(int i = 0; i < data.dataitemsCount; ++i) {
				reader.ReadType<msrd::data_item>(data.dataItems[i]);
				stream.seekg(0x8, std::istream::cur);
			}
		}

		if(data.textureIdsOffset != 0) {
			stream.seekg(data.offset + data.textureIdsOffset, std::istream::beg);
			data.textureIds.resize(data.textureIdsCount);

			for(int i = 0; i < data.textureIdsCount; ++i)
				reader.ReadType<int16>(data.textureIds[i]);
		}

		if(data.textureCountOffset != 0) {
			stream.seekg(data.offset + data.textureCountOffset, std::istream::beg);

			reader.ReadType<int32>(data.textureCount);
			reader.ReadType<int32>(data.textureChunkSize);
			reader.ReadType<int32>(data.unknown2);
			reader.ReadType<int32>(data.textureStringBufferOffset);

			data.textureInfo.resize(data.textureCount);
			
			for(int i = 0; i < data.textureCount; ++i) {
				reader.ReadType<msrd::texture_info>(data.textureInfo[i]);
			}

			data.textureNames.resize(data.textureCount);

			for(int i = 0; i < data.textureCount; ++i) {
				stream.seekg(data.offset + data.textureCountOffset + data.textureInfo[i].stringOffset, std::istream::beg);
				data.textureNames[i] = reader.ReadString();
			}
		}

		data.toc.resize(data.fileCount);
		
		for(int i = 0; i < data.fileCount; ++i) {
			stream.seekg(data.offset + data.tocOffset + (i * sizeof(msrd::toc_entry)), std::istream::beg);
			reader.ReadType<msrd::toc_entry>(data.toc[i]);

			PROGRESS_UPDATE(ProgressType::Verbose, "MSRD file " << i << ':')
			PROGRESS_UPDATE(ProgressType::Verbose, ".. is at decimal offset " << data.toc[i].offset)
			PROGRESS_UPDATE(ProgressType::Verbose, ".. is " << data.toc[i].compressedSize << " bytes compressed")
			PROGRESS_UPDATE(ProgressType::Verbose, ".. is " << data.toc[i].fileSize << " bytes uncompressed")

			// Decompress the xbc1 file
			xbc1Reader reader(stream);

			reader.set_progress(std::bind(&msrdReader::CheckedProgressUpdate, this, std::placeholders::_1, std::placeholders::_2));

			xbc1::xbc1 file = reader.Read({ data.toc[i].offset, opts.outputDirectory, opts.saveDecompressedXbc1 });

			if(!file.data.empty())
				data.files.push_back(file);

		}


		return data;
	}

}
}
