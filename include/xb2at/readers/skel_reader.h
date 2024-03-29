#include <xb2at/core.h>
#include <modeco/Logger.h>

#include <xb2at/structs/skel.h>

namespace xb2at {
	namespace core {

		enum class skelReaderStatus {
			Success,
			ErrorReadingHeader,
			NotSKEL,
		};

		inline std::string skelReaderStatusToString(skelReaderStatus status) {
			// avoiding magic const by using constexpr
			constexpr static const char* status_str[] = {
				"Success",
				"Error reading SKEL header",
				"File is not SKEL"
			};

			return status_str[(int)status];
		}

		/**
		 * Options to pass to skelReader::Read().
		 */
		struct skelReaderOptions {
			skelReaderOptions(std::vector<char>& fileData)
				: file(fileData) {
			}

			/**
		 * File data from SAR1.
		 */
			std::vector<char>& file;

			skelReaderStatus Result;
		};

		/**
		 * Reads and decompresses SAR1 files.
		 */
		struct skelReader {
			/**
			 * Read and decompress a singular SAR1 file.
			 *
			 * \param[in] opts Options to pass to the reader.
			 */
			skel::skel Read(skelReaderOptions& opts);

		   private:
			mco::Logger logger = mco::Logger::CreateLogger("SKELReader");
		};

	} // namespace core
} // namespace xb2at