#pragma once
#include <core.h>
#include <structs/mxmd.h>
#include "base_reader.h"

namespace xb2at {
	namespace core {

		/**
		 * Options for mxmdReader::Read()
		 */
		struct mxmdReaderOptions {
		};

		/**
		 * Reads meshes.
		 */
		struct mxmdReader : public base_reader {

			mxmdReader(std::istream& input_stream)
				: stream(input_stream) {

			}

			/**
			 * Read a MXMD file and output a structure.
			 *
			 * \param[in] opts Options to pass to the reader
			 */
			mxmd::mxmd Read(const mxmdReaderOptions& opts);


		private:
			std::istream& stream;
		};


	}
}