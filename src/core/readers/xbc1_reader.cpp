#include "xbc1_reader.h"
#include "streamhelper.h"
#include <zlib.h>

namespace xb2at {
namespace core {

	xbc1::xbc1 xbc1Reader::Read(xbc1ReaderOptions& opts) {
		StreamHelper reader(stream);
		xbc1::xbc1 xbc;

		logger.info("Reading XBC1 file at ", opts.offset);

		stream.seekg(opts.offset, std::istream::beg);

		if(!reader.ReadType<xbc1::header>(xbc)) {
			opts.Result = xbc1ReaderStatus::ErrorReadingHeader;
			return xbc;
		}

		if(strncmp(xbc.magic, "xbc1", sizeof(xbc.magic)) != 0) {
			opts.Result = xbc1ReaderStatus::NotXBC1;
			return xbc;
		}

		xbc.name = reader.ReadString();
		xbc.offset = opts.offset;
		
		xbc.data.resize(xbc.decompressedSize);
		stream.seekg(opts.offset + 0x30, std::istream::beg);

		std::vector<char> compressedData;
		compressedData.resize(xbc.compressedSize);
		stream.read(compressedData.data(), xbc.compressedSize);

		logger.verbose("Decompressing XBC1 data");

		// I suspect monolith was just as lazy as I am
		// and they just use compress() or compress2()
		int result = uncompress((Bytef*)xbc.data.data(), (uLongf*)&xbc.decompressedSize, (Bytef*)compressedData.data(), xbc.compressedSize);
		compressedData.clear();

		if(result != Z_OK) {
			logger.error("ZLib uncompress() returned ", result);
			// return zlib error state
			opts.Result = xbc1ReaderStatus::ZlibError;
			return xbc;
		}
		
		logger.verbose("Uncompressed XBC1 file data");

		// if the user wants to dump raw files
		// let them
		if(opts.save) {
			fs::path path(opts.output_dir);
			path = path / std::string("file_" + std::to_string(xbc.offset));
			path.replace_extension(".bin");

			logger.info("Writing uncompressed XBC1 to ", path.string());

			std::ofstream file(path.string(), std::ofstream::binary);

			file.write((char*)xbc.data.data(), xbc.decompressedSize);
			file.close();
		}

		opts.Result = xbc1ReaderStatus::Success;
		return xbc;
	}

}
}