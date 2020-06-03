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
			static const char* status_str[] = {
				"Success",
				"Error reading the LBIM header",
				"File is not a LBIM"
			};

			return status_str[(int)status];
		}

		/**
		 * Options to pass to the lbimReader.
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
			 * Start offset of the LBIM.
			 */
			int32 offset;

			/**
			 * Size of the texture data.
			 */
			int32 size;

			/**
			 * The result of the read operation.
			 * Will be lbimReaderStatus::Success if the read was successful.
			 */
			lbimReaderStatus Result;
		};

		/**
		 * Reads meshes.
		 */
		struct lbimReader : public base_reader {

			/**
			 * Read a LBIM and output the texture data.
			 * This function does not decompress/unswizzle any data.
			 *
			 * \param[in] opts Reference to options to pass to the reader.
			 */
			lbim::texture Read(lbimReaderOptions& opts);


		};


	}
}