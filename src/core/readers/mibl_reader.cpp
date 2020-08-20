#include "streamhelper.h"
#include "mibl_reader.h"

#include "ivstream.h"
#include <algorithm>

namespace xb2at {
	namespace core {

		mibl::texture miblReader::Read(miblReaderOptions& opts) {
			ivstream stream(opts.miblFile);
			StreamHelper reader(stream);
			mibl::texture texture;

			if(opts.file != nullptr) {
				texture.cached = false;
			} else {
				texture.cached = true;
			}

			stream.seekg(opts.offset + opts.size - sizeof(mibl::header), std::istream::beg);
			if(!reader.ReadType<mibl::header>(texture)) {
				opts.Result = miblReaderStatus::ErrorReadingHeader;
				return texture;
			}

			if(texture.magic != mibl::magic && texture.version != 0x2711) {
				opts.Result = miblReaderStatus::NotMIBL;
				return texture;
			}

#ifdef _DEBUG
			logger.info("MIBL type ", (int)texture.type);
#endif

			texture.data.resize(opts.size);

			if(!texture.cached) {
				// Use file for texture data as it's actually got it
				ivstream filestream(*opts.file);
				filestream.seekg(opts.offset, std::istream::beg);
				filestream.read(texture.data.data(), opts.size);

				// non cached means we need to *2 width and height for some reason
				texture.width *= 2;
				texture.height *= 2;
			} else {
				// this is a CachedTexture, use the LBIM stream itself
				stream.seekg(opts.offset, std::istream::beg);
				stream.read(texture.data.data(), opts.size);
			}

			opts.Result = miblReaderStatus::Success;
			return texture;
		}

	} // namespace core
} // namespace xb2at