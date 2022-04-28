#include <xb2at/readers/mibl_reader.h>

#include <xb2at/structs/xbc1.h>

#include <xb2at/core/ivstream.h>
#include <xb2at/core/IoStreamReadStream.h>
//#include <algorithm>

namespace xb2at::core {

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

			{
				ivstream miblStream(opts.miblFile);
				IoStreamReadStream readStream(miblStream);

				miblStream.seekg(opts.offset + opts.size - sizeof(mibl::header), std::istream::beg);

				// read the MIBL header
				if(!texture.header.Transform(readStream)) {
					opts.Result = miblReaderStatus::ErrorReadingHeader;
					return texture;
				}

				if(texture.header.magic != mibl::magic && texture.header.version != 0x2711) {
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
				texture.data.resize(opts.file->header.decompressedSize);
			else
				texture.data.resize(opts.size);

			if(!texture.cached) {
				// non cached textures use the passed-in xbc1

				stream->read(texture.data.data(), opts.file->header.decompressedSize);

				// non cached means we need to *2 width and height for some reason
				texture.header.width *= 2;
				texture.header.height *= 2;
			} else {
				// CachedTextures use the LBIM stream itself
				stream->seekg(opts.offset, std::istream::beg);
				stream->read(texture.data.data(), opts.size);
			}

			opts.Result = miblReaderStatus::Success;
			return texture;
		}

	} // namespace xb2at