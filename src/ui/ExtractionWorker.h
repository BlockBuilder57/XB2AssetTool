#pragma once
#include <QThread>

#include <core.h>

// Core reader/serializer API
#include <readers/msrd_reader.h>
#include <readers/mesh_reader.h>
#include <readers/mxmd_reader.h>
#include <readers/lbim_reader.h>
#include <readers/sar1_reader.h>
#include <readers/skel_reader.h>
#include <serializers/model_serializer.h>

// TODO this should prrrrobably be renamed
#include <tegra_texture.h>

using namespace xb2at::core;

namespace xb2at {
	namespace ui {

		struct ExtractionWorkerOptions {
			bool saveTextures;
			bool saveMorphs;
			bool saveAnimations;
			bool saveOutlines;
			modelSerializerOptions::Format modelFormat;
			int32 lod;

			bool saveMapMesh;
			bool saveMapProps;
			int32 propSplitSize;

			bool saveXBC1;
		};

		/**
		 * Extraction worker.
		 */
		class ExtractionWorker : public QObject {
			Q_OBJECT

		   public:
			inline ~ExtractionWorker() {
				// Simply emit the Finished() signal
				// so that the UI knows we're done.
				emit Finished();
			}

			// Util functions

			void MakeDirectoryIfNotExists(fs::path& root, const std::string& directoryName);

			void LogCallback(std::string message, LogSeverity type);

			// read seperation functions

			/**
			 * Read MSRD file and output a structure.
			 * Returns true on success, false otherwise.
			 * 
			 * \param[in] path Path to MSRD file.
			 * \param[in] msrdtoReadTo The core MSRD structure to read to
			 * \param[in] options Options to pass to the reader.
			 */
			bool ReadMSRD(fs::path& path, msrd::msrd& msrdToReadTo, msrdReaderOptions& options);

			/**
			 * Read MXMD file and output a structure.
			 * Returns true on success, false otherwise.
			 * 
			 * \param[in] path Path to MXMD file.
			 * \param[in] mxmdtoReadTo The core MXMD structure to read to
			 * \param[in] options Options to pass to the reader.
			 */
			bool ReadMXMD(fs::path& path, mxmd::mxmd& mxmdToReadTo, mxmdReaderOptions& options);

			/**
			 * Read SAR1 file and output a structure.
			 * Returns true on success, false otherwise.
			 * 
			 * \param[in] path Path to SAR1 file.
			 * \param[in] sar1toReadTo The core SAR1 structure to read to
			 * \param[in] options Options to pass to the reader.
			 */
			bool ReadSAR1(fs::path& path, const std::string& extension, sar1::sar1& sar1ToReadTo, sar1ReaderOptions& options);

			/**
			 * Read SKEL file and output a structure.
			 * Returns true on success, false otherwise.
			 * 
			 * \param[in] path Path to SKEL file.
			 * \param[in] skeltoReadTo The core SKEL structure to read to
			 * \param[in] options Options to pass to the reader.
			 */
			bool ReadSKEL(fs::path& path, skel::skel& skelToReadto);

			// MSRD read functions

			/**
			 * Reads a mesh
			 *
			 * \param[in] mesh Reference to mesh to read into
			 * \param[in] options Options to pass to reader
			 */
			bool ReadMesh(mesh::mesh& mesh, meshReaderOptions& options);

			/**
			 * Reads a LBIM texture
			 *
			 * \param[in] texture Reference to texture to read into
			 * \param[in] options Options to pass to reader
			 */
			bool ReadLBIM(lbim::texture& texture, lbimReaderOptions& options);

			bool SerializeLBIM(fs::path& outputPath, lbim::texture& texture);

			// TODO(lily): we'll probably have to give this texture access too
			/**
			 * Serializes meshes.
			 *
			 * \param[in] meshesToDump Vector of meshes to convert
			 * \param[in] mxmdData MXMD data
			 * \param[in] skelData SKEL data
			 * \param[in] options Options to pass to model serializer
			 */
			void SerializeMesh(std::vector<mesh::mesh>& meshesToDump, mxmd::mxmd& mxmdData, skel::skel& skelData, modelSerializerOptions& options);

			/**
			 * Perform complete extraction of assets.
			 *
			 * \param[in] filename Base filename to use
			 * \param[in] outputPath the output path to use
			 * \param[in] options optioms to use
			 */
			void ExtractAll(std::string& filename, fs::path& outputPath, ExtractionWorkerOptions& options);

			/**
			 * This is a very, very very, VERY bad hack to avoid memory leaking.
			 * We delete ourselves when we're done with a secondary purpose of also emitting the finished signal.
			 */
			inline void Done() {
				delete this;
			}

			Logger logger = Logger::CreateChannel("ExtractionWorker");

		   signals:
			void LogMessage(QString message, LogSeverity type = LogSeverity::Info);
			void Finished();
		};

	} // namespace ui
} // namespace xb2at