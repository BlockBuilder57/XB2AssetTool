#include "streamhelper.h"
#include "mibl_reader.h"

#include "ivstream.h"
#include <algorithm>

namespace xb2at {
	namespace core {

		mibl::texture miblReader::Read(miblReaderOptions& opts) {
			std::shared_ptr<ivstream> stream;
			mibl::texture texture;

			if(opts.file != nullptr) {
				texture.cached = false;
				stream = std::make_shared<ivstream>(opts.file->data);
			} else {
				texture.cached = true;
				stream = std::make_shared<ivstream>(opts.miblFile);
			}

			// initiate a new scope here cause these objects are only ever used once
			{
				ivstream miblStream(opts.miblFile);
				StreamHelper reader(miblStream);

				miblStream.seekg(opts.offset + opts.size - sizeof(mibl::header), std::istream::beg);

				if(!reader.ReadType<mibl::header>(texture)) {
					opts.Result = miblReaderStatus::ErrorReadingHeader;
					return texture;
				}

				if(texture.magic != mibl::magic && texture.version != 0x2711) {
					opts.Result = miblReaderStatus::NotMIBL;
					return texture;
				}
			}

			// TEMP
#ifdef _DEBUG
			logger.info("MIBL type ", (int)texture.type);
#endif

			// resize to the appropriate size depending on if the texture
			// is a CachedTexture or not
			if(!texture.cached)
				texture.data.resize(opts.file->decompressedSize);
			else
				texture.data.resize(opts.size);

			if(!texture.cached) {
				// non cached textures use the passed-in xbc1

				stream->read(texture.data.data(), opts.file->decompressedSize);

				// non cached means we need to *2 width and height for some reason
				texture.width *= 2;
				texture.height *= 2;
			} else {
				// CachedTextures use the LBIM stream itself
				stream->seekg(opts.offset, std::istream::beg);
				stream->read(texture.data.data(), opts.size);
			}

			opts.Result = miblReaderStatus::Success;
			return texture;
		}

	} // namespace core
} // namespace xb2at