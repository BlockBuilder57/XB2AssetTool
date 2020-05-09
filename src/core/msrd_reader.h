#pragma once
#include <core.h>
#include <structs.h>
#include "base_reader.h"

namespace xb2at {
namespace core {

	struct msrdReaderOptions {
		std::string outputDirectory;
		bool saveDecompressedXbc1;
	};

	struct msrdReader : public base_reader {

		msrdReader(std::istream& input_stream)
			: stream(input_stream) {
		
		}

		msrd::msrd Read(msrdReaderOptions& opts);
		

	private:
		std::istream& stream;
	};


}
}