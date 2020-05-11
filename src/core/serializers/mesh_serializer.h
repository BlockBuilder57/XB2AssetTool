#pragma once
#include <core.h>
#include <structs.h>

namespace xb2at {
namespace core {

	/**
	 * options to pass to mesh::
	 */
	struct meshSerializerOptions {

		/**
		 * Output format.
		 */
		enum class Format : byte {
			/**
			 * binary glTF format
			 */
			GLTFBinary,

			/**
			 * Raw dump format.
			 */
			Dump
		} OutputFormat;

		std::string outputDir;

		std::string filename;
	};

	/**
	 * Mesh serializer.
	 * Serializes mesh to a file.
	 */
	struct meshSerializer {

		void Serialize(mesh::mesh& meshToDump, const meshSerializerOptions& options);

	};
}
}