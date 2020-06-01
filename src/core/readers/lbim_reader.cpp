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

		PROGRESS_UPDATE(ProgressType::Warning, "lbimReader::Read(): stub")

		opts.Result = lbimReaderStatus::Success;
		return texture;
	}

}
}