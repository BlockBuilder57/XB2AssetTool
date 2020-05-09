#include <core.h>
#include <structs.h>
#include "base_reader.h"

namespace xb2at {
namespace core {

	/**
	 * Like its name implies, reads XBC1's.
	 */
	struct xbc1Reader : public base_reader  {

		xbc1Reader(std::istream& input_stream)
			: stream(input_stream) {
		
		}

		xbc1::xbc1 Read(int32 offset, std::string output_dir, bool save);
		

	private:
		std::istream& stream;
	};


}
}