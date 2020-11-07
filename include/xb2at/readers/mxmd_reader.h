#pragma once
#include <xb2at/core.h>
#include <modeco/Logger.h>

#include <xb2at/structs/mxmd.h>

namespace xb2at {
	namespace core {

		enum class mxmdReaderStatus {
			Success,
			ErrorReadingHeader,
			NotMXMD
		};

		inline std::string mxmdReaderStatusToString(mxmdReaderStatus status) {
			// avoiding magic const by using constexpr
			constexpr static const char* status_str[] = {
				"Success",
				"Error reading MXMD header",
				"File is not a MXMD file"
			};

			return status_str[(int)status];
		}

		/**
		 * Options for mxmdReader::Read()
		 */
		struct mxmdReaderOptions {
			mxmdReaderStatus Result;
		};

		/**
		 * Reads meshes.
		 */
		struct mxmdReader {
			mxmdReader(std::istream& input_stream)
				: stream(input_stream) {
			}

			/**
			 * Read a MXMD file and output a structure.
			 *
			 * \param[in] opts Options to pass to the reader
			 */
			mxmd::mxmd Read(mxmdReaderOptions& opts);

		   private:
			std::istream& stream;

			mco::Logger logger = mco::Logger::CreateLogger("MXMDReader");
		};

	} // namespace core
} // namespace xb2at