#include "msrd_reader.h"
#include "streamhelper.h"
#include "xbc1_reader.h"
#include "mesh_reader.h"

namespace xb2at {
	namespace core {

		msrd::msrd msrdReader::Read(msrdReaderOptions& opts) {
			StreamHelper reader(stream);
			msrd::msrd data;

			// Read the initial header
			if(!reader.ReadType<msrd::msrd_header>(data)) {
				opts.Result = msrdReaderStatus::ErrorReadingHeader;
				return data;
			}

			if(strncmp(data.magic, "DRSM", sizeof(data.magic)) != 0) {
				opts.Result = msrdReaderStatus::NotMSRD;
				return data;
			}

			logger.verbose("MSRD version: ", data.version);

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

				// display some information about the MSRD file when verbose logging
				logger.verbose("MSRD file ", i, ':');
				logger.verbose(".. is at offset (decimal) ", data.toc[i].offset);
				logger.verbose(".. is ", data.toc[i].compressedSize, " bytes compressed");
				logger.verbose(".. is ", data.toc[i].fileSize, " bytes uncompressed");

				// Decompress the xbc1 file (this may/will be moved to the extraction worker).

				xbc1Reader reader(stream);

				xbc1ReaderOptions options = {
					data.toc[i].offset,
					opts.outputDirectory,
					opts.saveDecompressedXbc1
				};

				xbc1::xbc1 file = reader.Read(options);

				if(options.Result == xbc1ReaderStatus::Success) {
					data.files.push_back(file);
				} else {
					logger.error("Error reading XBC1 file ", i, ": ", xbc1ReaderStatusToString(options.Result));
				}
			}

			opts.Result = msrdReaderStatus::Success;
			return data;
		}

	} // namespace core
} // namespace xb2at
