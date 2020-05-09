#include "xbc1_reader.h"
#include "streamhelper.h"
#include <zlib.h>

namespace xb2at {
namespace core {

	xbc1::xbc1 xbc1Reader::Read(int32 offset, std::string output_dir, bool save) {
		StreamHelper reader(stream);
		xbc1::xbc1 xbc;

		stream.seekg(offset, std::istream::beg);

		if(!reader.ReadType<xbc1::xbc1_header>(xbc)) {
			CheckedProgressUpdate("could not read XBC1 header", ProgressType::Error);
			return xbc;
		}

		if(!strncmp(xbc.magic, "xbc1", sizeof("xbc1"))) {
			CheckedProgressUpdate("not XBC1 data", ProgressType::Error);
			return xbc;
		}

		xbc.name = reader.ReadString();
		xbc.offset = offset;
		
		xbc.data.resize(xbc.decompressedSize);
		stream.seekg(offset + 0x30, std::istream::beg);

		std::vector<char> compressedData;
		compressedData.resize(xbc.compressedSize);
		stream.read(compressedData.data(), xbc.compressedSize);

		
		CheckedProgressUpdate("Decompressing XBC1 data", ProgressType::Verbose);
		int result = uncompress((Bytef*)xbc.data.data(), (uLongf*)&xbc.decompressedSize, (Bytef*)compressedData.data(), xbc.compressedSize);
		CheckedProgressUpdate("Decompressed", ProgressType::Verbose);

		compressedData.clear();

		if(result != Z_OK) {
			CheckedProgressUpdate("zlib error: uncompress() returned " + std::to_string(result), ProgressType::Error);
			return xbc;
		}

		if(save) {
			std::stringstream ss;
			ss << output_dir << "/file_" << offset << ".bin";

			CheckedProgressUpdate("Writing raw XBC1 file to" + ss.str() + "...", ProgressType::Verbose);
			std::ofstream file(ss.str(), std::ofstream::binary);

			file.write((char*)xbc.data.data(), xbc.decompressedSize);
			file.close();
			ss.clear();
		}

		return xbc;
	}

}
}