#pragma once
#include <xb2at/core.h>

#include <xb2at/structs/msrd.h>

namespace xb2at {
	namespace core {

		enum class msrdReaderStatus {
			Success,
			GeneralReadError,
			ErrorReadingHeader,
			NotMSRD,
		};

		inline std::string msrdReaderStatusToString(msrdReaderStatus status) {
			// avoiding magic const by using constexpr
			constexpr static const char* status_str[] = {
				"Success",
				"General read error"
				"Error reading MSRD header",
				"File is not a MSRD file"
			};

			return status_str[(int)status];
		}

		/**
		 * Options to pass to msrdReader::Read().
		 */
		struct msrdReaderOptions {
			// TODO: Move the following two members to the XBC1 reader options structure

			/**
			 * The output directory (where we should place files.)
			 */
			fs::path outputDirectory;

			/**
			 * Save decompressed XBC1 files to the output directory.
			 */
			bool saveDecompressedXbc1;

			msrdReaderStatus Result;
		};

		/**
		 * Reads MSRD files.
		 */
		struct msrdReader {
			msrdReader(std::istream& input_stream)
				: stream(input_stream) {
			}

			/**
			 * Read a MSRD file and output a structure.
			 *
			 * \param[in] opts Options to pass to the reader
			 */
			msrd::msrd Read(msrdReaderOptions& opts);

		   private:
			std::istream& stream;

			//mco::Logger logger = mco::Logger::CreateLogger("MSRDReader");
		};

	} // namespace core
} // namespace xb2at