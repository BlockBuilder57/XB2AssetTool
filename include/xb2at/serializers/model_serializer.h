#pragma once
#include <xb2at/core.h>
#include <modeco/Logger.h>

#include <xb2at/structs/mesh.h>
#include <xb2at/structs/mxmd.h>
#include <xb2at/structs/mibl.h>
#include <xb2at/structs/skel.h>

namespace xb2at {
	namespace core {

		/**
		 * options to pass to modelSerializer::Serialize()
		 */
		struct modelSerializerOptions {
			/**
			 * Output format.
			 */
			enum class Format : byte {
				/**
				 * Binary glTF format.
				 */
				GLTFBinary,

				/**
				 * Text glTF format.
				 */
				GLTFText
			} OutputFormat;

			/**
			 * Output directory.
			 */
			const fs::path& outputDir;

			/**
			 * Output filename of the model.
			 */
			const std::string& filename;

			/**
			 * The level of detail at which we should export meshes.
			 */
			int32 lod;

			/**
			 * Save morphs to the model?
			 */
			bool saveMorphs;

			/**
			 * Save outlines to the model?
			 */
			bool saveOutlines;
		};

		/**
		 * Model serializer.
		 * Serializes model to a file.
		 */
		struct modelSerializer {
			/**
			 * Serialize a mesh with the provided options,
			 *
			 * \param[in] meshesToDump The meshes to dump.
			 * \param[in] options Options.
			 */
			void Serialize(std::vector<mesh::mesh>& meshToDump, mxmd::mxmd& mxmdData, skel::skel& skelData, modelSerializerOptions& options);

		   private:
			mco::Logger logger = mco::Logger::CreateLogger("ModelSerializer");
		};
	} // namespace core
} // namespace xb2at