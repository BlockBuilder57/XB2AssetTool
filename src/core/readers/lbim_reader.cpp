#include "streamhelper.h"
#include "endian_swap.h"
#include "lbim_reader.h"

#include "ivstream.h"
#include <algorithm>

namespace xb2at {
namespace core {

	lbim::texture lbimReader::Read(lbimReaderOptions& opts) {
		ivstream lbimstream(opts.lbimFile);
		StreamHelper lbimreader(lbimstream);
		lbim::texture texture;

		lbimstream.seekg(opts.offset + opts.size - sizeof(lbim::header), std::istream::beg);
		if(!lbimreader.ReadType<lbim::header>(texture)) {
			opts.Result = lbimReaderStatus::ErrorReadingHeader;
			return texture;
		}

		if(texture.magic != lbim::magic) {
			opts.Result = lbimReaderStatus::NotLBIM;
			return texture;
		}

		texture.offset = opts.offset;
		texture.data.resize(opts.size);

		if(opts.file != nullptr) {
			// Use file for texture data
			ivstream filestream(*opts.file);
			filestream.seekg(opts.offset, std::istream::beg);
			filestream.read(texture.data.data(), opts.size);
		} else {
			// CachedTexture, use the LBIM stream itself
			lbimstream.seekg(opts.offset, std::istream::beg);
			lbimstream.read(texture.data.data(), opts.size);
		}

		opts.Result = lbimReaderStatus::Success;
		return texture;
	}

}
}