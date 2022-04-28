#pragma once

#include <memory>
#include <xb2at/structs/mibl.h>
#include <string>
#include <vector>

namespace xb2at::core {

		// fwd decl
		struct Xbc1;

		enum class miblReaderStatus {
			Success,
			ErrorReadingHeader,
			NotMIBL
		};

		inline std::string miblReaderStatusToString(miblReaderStatus status) {
			// avoiding magic const by using constexpr
			constexpr static const char* status_str[] = {
				"Success",
				"Error reading the MIBL header",
				"File is not a MIBL file, or the version is invalid"
			};

			return status_str[(int)status];
		}

		/**
		 * Options to pass to the miblReader.
		 */
		struct miblReaderOptions {

			// TODO(lily): rename some of these members so they're not as confusing

			miblReaderOptions(std::vector<std::uint8_t>& miblFileData, Xbc1* fileData)
				: miblFile(miblFileData) {
				file = fileData;
			}

			/**
			 * Decompressed file data from XBC1 containing every MIBL for the MSRD textures.
			 * Is always index 1 of the MSRD files.
			 */
			std::vector<std::uint8_t>& miblFile;

			/**
			 * Decompressed texture data as an XBC1 file.
			 * If this is provided the reader will assume this is not a cached texture.
			 */
			Xbc1* file;

			/**
			 * Start offset of the texture.
			 */
			std::uint32_t offset{};

			/**
			 * Size of the texture data.
			 */
			std::uint32_t size{};

			/**
			 * The result of the read operation.
			 * Will be miblReaderStatus::Success if the read was successful, any other
			 * value to indicate a varying error state.
			 */
			miblReaderStatus Result{};
		};

		/**
		 * Reads MIBL textures.
		 */
		struct miblReader {

			/**
			 * Read a MIBL and output the texture data.
			 * This function does not decompress/unswizzle any data.
			 *
			 * \param[in] opts Reference to options to pass to the reader.
			 */
			mibl::texture Read(miblReaderOptions& opts);

		private:

			//mco::Logger logger = mco::Logger::CreateLogger("MIBLReader");
		};


	}