#include "streamhelper.h"
#include "lbim_reader.h"

#include "ivstream.h"
#include <algorithm>

namespace xb2at {
	namespace core {

		lbim::texture lbimReader::Read(lbimReaderOptions& opts) {
			ivstream lbimstream(opts.lbimFile);
			StreamHelper lbimreader(lbimstream);
			lbim::texture texture;

			if(opts.file != nullptr) {
				texture.cached = false;
			} else {
				texture.cached = true;
			}

			lbimstream.seekg(opts.offset + opts.size - sizeof(lbim::header), std::istream::beg);
			if(!lbimreader.ReadType<lbim::header>(texture)) {
				opts.Result = lbimReaderStatus::ErrorReadingHeader;
				return texture;
			}

			if(texture.magic != lbim::magic && texture.version != 0x2711) {
				opts.Result = lbimReaderStatus::NotLBIM;
				return texture;
			}

#ifdef _DEBUG
			logger.info("LBIM type ", (int)texture.type);
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
				lbimstream.seekg(opts.offset, std::istream::beg);
				lbimstream.read(texture.data.data(), opts.size);
			}

			opts.Result = lbimReaderStatus::Success;
			return texture;
		}

	} // namespace core
} // namespace xb2at