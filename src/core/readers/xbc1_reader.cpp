#include "xbc1_reader.h"
#include "streamhelper.h"
#include <zlib.h>

namespace xb2at {
namespace core {

	xbc1::xbc1 xbc1Reader::Read(const xbc1ReaderOptions& opts) {
		StreamHelper reader(stream);
		xbc1::xbc1 xbc;

		CheckedProgressUpdate("Attempting to read XBC1 at " + std::to_string(opts.offset), ProgressType::Info);
		stream.seekg(opts.offset, std::istream::beg);

		if(!reader.ReadType<xbc1::xbc1_header>(xbc)) {
			CheckedProgressUpdate("could not read XBC1 header", ProgressType::Error);
			return xbc;
		}

		if(!strncmp(xbc.magic, "xbc1", sizeof("xbc1"))) {
			CheckedProgressUpdate("not XBC1 data", ProgressType::Error);
			return xbc;
		}

		xbc.name = reader.ReadString();
		xbc.offset = opts.offset;
		
		xbc.data.resize(xbc.decompressedSize);
		stream.seekg(opts.offset + 0x30, std::istream::beg);

		std::vector<char> compressedData;
		compressedData.resize(xbc.compressedSize);
		stream.read(compressedData.data(), xbc.compressedSize);

		
		CheckedProgressUpdate("Decompressing XBC1 data", ProgressType::Verbose);

		// I suspect monolith was just as lazy as I am
		// and they just use compress() or compress2()
		int result = uncompress((Bytef*)xbc.data.data(), (uLongf*)&xbc.decompressedSize, (Bytef*)compressedData.data(), xbc.compressedSize);

		compressedData.clear();

		if(result != Z_OK) {
			CheckedProgressUpdate("zlib error: uncompress() returned " + std::to_string(result), ProgressType::Error);
			return xbc;
		}
		
		CheckedProgressUpdate("Decompressed data successfully", ProgressType::Verbose);

		// if the user wants to save raw files
		if(opts.save) {
			fs::path path = opts.output_dir;
			path = path / std::string("file_" + std::to_string(xbc.offset) + ".bin");

			CheckedProgressUpdate("Writing raw XBC1 file to " + path.string() + "...", ProgressType::Verbose);
			std::ofstream file(path.string(), std::ofstream::binary);

			file.write((char*)xbc.data.data(), xbc.decompressedSize);
			file.close();
		}

		return xbc;
	}

}
}