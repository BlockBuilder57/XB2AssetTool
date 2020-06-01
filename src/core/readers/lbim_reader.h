#pragma once
#include <core.h>
#include <structs/lbim.h>
#include "base_reader.h"

namespace xb2at {
namespace core {

		enum class lbimReaderStatus {
			Success,
			ErrorReadingHeader,
			NotLBIM
		};

		inline std::string lbimReaderStatusToString(lbimReaderStatus status) {
			const char* status_str[] = {
				"Success",
				"Error reading LBIM header",
				"File is not a LBIM"
			};

			return status_str[(int)status];
		}

		/**
		 * Options for lbimReader::Read()
		 */
		struct lbimReaderOptions {

			lbimReaderOptions(std::vector<char>& fileData)
				: file(fileData) {

			}

			/**
			 * Decompressed file data from XBC1.
			 */
			std::vector<char>& file;

			/**
			 * The result of the read operation.
			 */
			lbimReaderStatus Result;
		};

		/**
		 * Reads meshes.
		 */
		struct lbimReader : public base_reader {

			/**
			 * Read a mesh and output a deserialized structure.
			 *
			 * \param[in] opts Options.
			 */
			lbim::texture Read(lbimReaderOptions& opts);


		};


	}
}