#include <xb2at/core.h>

#include <xb2at/structs/xbc1.h>

namespace xb2at {
	namespace core {

		enum class xbc1ReaderStatus {
			Success,
			ErrorReadingHeader,
			NotXBC1,
			ZlibError
		};

		inline std::string xbc1ReaderStatusToString(xbc1ReaderStatus status) {
			// using constexpr to avoid magic const overhead
			constexpr static const char* status_str[] = {
				"Success",
				"Error reading XBC1 header",
				"File is not XBC1",
				"Error decompressing Zlib data"
			};

			return status_str[(int)status];
		}

		/**
		 * Options to pass to xbc1Reader::Read().
		 */
		struct xbc1ReaderOptions {
			/**
			 * Offset where XBC1 file starts.
			 */
			std::uint32_t offset;

			/**
			 * Output directory.
			 */
			fs::path output_dir;

			/**
			 * Whether or not we should save files in the output directory.
			 */
			bool save;

			xbc1ReaderStatus Result;
		};

		/**
		 * Reads and decompresses XBC1 files.
		 */
		struct xbc1Reader {
			xbc1Reader(std::istream& input_stream)
				: stream(input_stream) {
			}

			/**
			 * Read and decompress a singular XBC1 file.
			 *
			 * \param[in] opts Options to pass to the reader.
			*/
			Xbc1 Read(xbc1ReaderOptions& opts);

		   private:
			std::istream& stream;

			//mco::Logger logger = mco::Logger::CreateLogger("XBC1Reader");
		};

	} // namespace core
} // namespace xb2at