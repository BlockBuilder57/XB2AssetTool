#include <xb2at/readers/msrd_reader.h>

#include <xb2at/core/IoStreamReadStream.h>

#include <xb2at/readers/xbc1_reader.h>
//#include <xb2at/readers/mesh_reader.h>

namespace xb2at::core {

	msrd::Msrd msrdReader::Read(msrdReaderOptions& opts) {
		IoStreamReadStream readStream(stream);
		msrd::Msrd data;

		if(data.header.Transform(readStream)) {
			opts.Result = msrdReaderStatus::ErrorReadingHeader;
			return data;
		}

		if(strncmp(data.header.magic, "DRSM", sizeof(data.header.magic)) != 0) {
			opts.Result = msrdReaderStatus::NotMSRD;
			return data;
		}

		//logger.verbose("MSRD version: ", data.version);

		if(data.header.dataitemsOffset != 0) {
			readStream.Seek(StreamSeekDir::Begin, data.header.offset + data.header.dataitemsOffset);
			data.dataItems.resize(data.header.dataitemsCount);

			for(auto& di : data.dataItems) {
				if(!di.Transform(readStream)) {
					opts.Result = msrdReaderStatus::GeneralReadError;
					return data;
				}
			}
		}

		if(data.header.textureIdsOffset != 0) {
			readStream.Seek(StreamSeekDir::Begin, data.header.offset + data.header.textureIdsOffset);

			if(!readStream.Array<std::endian::little, std::uint16_t>(data.header.textureIdsCount, data.textureIds)) {
				opts.Result = msrdReaderStatus::GeneralReadError;
				return data;
			}
		}

		if(data.header.textureCountOffset != 0) {
			readStream.Seek(StreamSeekDir::Begin, data.header.offset + data.header.textureCountOffset);

			readStream.GivenType<std::endian::little>(data.textureCount);
			readStream.GivenType<std::endian::little>(data.textureChunkSize);
			readStream.GivenType<std::endian::little>(data.unknown2);
			readStream.GivenType<std::endian::little>(data.textureStringBufferOffset);

			readStream.GivenType<std::endian::little>(data.textureCount);
			data.textureInfo.resize(data.textureCount);

			for(auto& texture : data.textureInfo)
				if(!texture.Transform(readStream)) {
					opts.Result = msrdReaderStatus::GeneralReadError;
					return data;
				}

			// This only really exists because GivenType<Endian, std::string> doesn't invoke String().
			// If it did, we could just use Array<Endian, std::String>
			data.textureNames.resize(data.textureCount);

			for(int i = 0; i < data.textureCount; ++i) {
				readStream.Seek(StreamSeekDir::Begin, data.header.offset + data.header.textureCountOffset + data.textureInfo[i].stringOffset);
				if(!readStream.String(data.textureNames[i])) {
					opts.Result = msrdReaderStatus::GeneralReadError;
					return data;
				}
			}
		}

		data.toc.resize(data.header.fileCount);

		for(int i = 0; i < data.header.fileCount; ++i) {
			readStream.Seek(StreamSeekDir::Begin, data.header.offset + data.header.tocOffset + (i * sizeof(msrd::TocEntry)));

			if(!data.toc[i].Transform(readStream)) {
				opts.Result = msrdReaderStatus::GeneralReadError;
				return data;
			}

			// display some information about the MSRD file when verbose logging
			//logger.verbose("MSRD TOC file ", i, ':');
			//logger.verbose(".. is at offset (decimal) ", data.toc[i].offset);
			//logger.verbose(".. is ", data.toc[i].compressedSize, " bytes compressed");
			//logger.verbose(".. is ", data.toc[i].fileSize, " bytes uncompressed");

			// Decompress the xbc1 file (this may/will be moved to the extraction worker).

			xbc1Reader reader(stream);

			xbc1ReaderOptions options = {
				data.toc[i].offset,
				opts.outputDirectory,
				opts.saveDecompressedXbc1
			};

			Xbc1 file = reader.Read(options);

			if(options.Result == xbc1ReaderStatus::Success) {
				data.files.push_back(file);
			} else {
				//logger.error("Error reading XBC1 file ", i, ": ", xbc1ReaderStatusToString(options.Result));
			}
		}

		opts.Result = msrdReaderStatus::Success;
		return data;
	}

} // namespace xb2at::core
