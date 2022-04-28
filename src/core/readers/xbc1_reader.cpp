#include <xb2at/readers/xbc1_reader.h>
#include <xb2at/streamhelper.h>

#include <xb2at/core/IoStreamReadStream.h>
#include <zlib.h>

namespace xb2at::core {

	Xbc1 xbc1Reader::Read(xbc1ReaderOptions& opts) {
		IoStreamReadStream stream(this->stream);
		Xbc1 xbc {
			.offset = opts.offset
		};

		//logger.info("Reading XBC1 file at ", opts.offset);
		stream.Seek(StreamSeekDir::Begin, opts.offset);

		if(!xbc.header.Transform(stream)) {
			opts.Result = xbc1ReaderStatus::ErrorReadingHeader;
			return xbc;
		}

		if(xbc.header.magic != Xbc1::magic) {
			opts.Result = xbc1ReaderStatus::NotXBC1;
			return xbc;
		}

		std::vector<std::uint8_t> compressedData(xbc.header.compressedSize);

		// Read the compressed data into the temporary buffer (without using the Stream concept tools)
		stream.Seek(StreamSeekDir::Begin, opts.offset + 0x30);
		stream.GetStream().read(reinterpret_cast<char*>(xbc.data.data()), xbc.header.compressedSize);

		//logger.verbose("Decompressing XBC1 data");

		xbc.data.resize(xbc.header.decompressedSize);

		// I suspect monolith was just as lazy as I am
		// nevermind, they use low level inflate*() actually
		int result = uncompress(xbc.data.data(), (uLongf*)&xbc.header.decompressedSize, compressedData.data(), xbc.header.compressedSize);
		compressedData.clear();

		if(result != Z_OK) {
			//logger.error("ZLib uncompress() returned ", result);
			// return zlib error state
			opts.Result = xbc1ReaderStatus::ZlibError;
			return xbc;
		}

		//logger.verbose("Uncompressed XBC1 file data");

		// if the user wants to dump raw files
		// dump out the xbc1 to a file in the output/Dump directory
		if(opts.save) {
			fs::path path(opts.output_dir);
			path = path / std::string("file_" + std::to_string(xbc.offset));
			path.replace_extension(".bin");

			//logger.info("Writing uncompressed XBC1 to ", path.string());

			std::ofstream file(path.string(), std::ofstream::binary);

			file.write(reinterpret_cast<char*>(xbc.data.data()), xbc.header.decompressedSize);
			file.close();
		}

		opts.Result = xbc1ReaderStatus::Success;
		return xbc;
	}

} // namespace xb2at::core