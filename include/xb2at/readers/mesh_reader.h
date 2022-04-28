#pragma once
#include <xb2at/core.h>
#include <modeco/Logger.h>

#include <xb2at/structs/mesh.h>

namespace xb2at {
namespace core {
	
	enum class meshReaderStatus {
		Success,
		ErrorReadingHeader,
		ErrorReadingVertexData,
		ErrorReadingFaceData,
		ErrorReadingWeightData,
		ErrorReadingMorphData
	};

	inline std::string meshReaderStatusToString(meshReaderStatus status) {
		// avoiding magic const by using constexpr
		constexpr static const char* status_str[] = {
			"Success",
			"Error reading mesh header",
			"Error reading vertex data",
			"Error reading face data",
			"Error reading weight data",
			"Error reading morph data",
		};

		return status_str[(int)status];
	}

	/**
	 * Options for meshReader::Read()
	 */
	struct meshReaderOptions {

		meshReaderOptions(std::vector<char>& fileData) 
			: file(fileData) {
			
		}

		/**
		 * Decompressed file data from XBC1.
		 */
		std::vector<char>& file;

		/**
		 * The result of the read operation.
		 */
		meshReaderStatus Result;
	};

	/**
	 * Reads meshes.
	 */
	struct meshReader {

		/**
		 * Read a mesh and output a deserialized structure.
		 *
		 * \param[in] opts Options.
		 */
		mesh::mesh Read(meshReaderOptions& opts);
		
		mco::Logger logger = mco::Logger::CreateLogger("MeshReader");
	};


}
}