#pragma once
#include <core.h>
#include <structs.h>
#include <serializers/base_serializer.h>

namespace xb2at {
namespace core {

	/**
	 * options to pass to meshSerializer::Serialize()
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
			 * text glTF format
			 */
			 GLTFText,

			/**
			 * Raw dump format.
			 */
			Dump
		} OutputFormat;
		
		/**
		 * Output directory.
		 */
		fs::path outputDir;

		/**
		 * Output filename of the mesh.
		 */
		std::string filename;
	};

	/**
	 * Mesh serializer.
	 * Serializes mesh to a file.
	 */
	struct meshSerializer : public base_serializer {

		/**
		 * Serialize a mesh with the provided options,
		 *
		 * \param[in] meshToDump The mesh to dump.
		 * \param[in] options Options.
		 */
		void Serialize(mesh::mesh& meshToDump, const meshSerializerOptions& options);

	};
}
}