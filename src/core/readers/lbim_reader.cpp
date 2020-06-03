#include "streamhelper.h"
#include "endian_swap.h"
#include "lbim_reader.h"

#include "ivstream.h"
#include <algorithm>

namespace xb2at {
namespace core {

	lbim::texture lbimReader::Read(lbimReaderOptions& opts) {
		ivstream stream(opts.file);
		StreamHelper reader(stream);
		lbim::texture texture;

		stream.seekg(opts.size - sizeof(lbim::header), std::istream::beg);
		if(!reader.ReadType<lbim::header>(texture)) {
			opts.Result = lbimReaderStatus::ErrorReadingHeader;
			return texture;
		}

		if(texture.magic != lbim::magic) {
			opts.Result = lbimReaderStatus::NotLBIM;
			return texture;
		}

		texture.offset = opts.offset;

		texture.data.resize(opts.size);

		stream.seekg(opts.offset, std::istream::beg);
		stream.read(texture.data.data(), opts.size);

		opts.Result = lbimReaderStatus::Success;
		return texture;
	}

}
}