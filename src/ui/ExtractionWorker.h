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

		bool ReadMSRD(fs::path& path, msrd::msrd& msrdToReadTo, msrdReaderOptions& options);

		bool ReadMXMD(fs::path& path, mxmd::mxmd& mxmdToReadTo, mxmdReaderOptions& options);

		bool ReadSAR1(fs::path& path, const std::string& extension, sar1::sar1& sar1ToReadTo, sar1ReaderOptions& options);

		bool ReadSKEL(fs::path& path, skel::skel& skelToReadto);
		

		void DoIt(std::string& filename, fs::path& outputPath, ExtractionWorkerOptions& options);

		/**
		 * This is a very bad hack to avoid memory leaking.
		 * We delete ourselves when we're done with a secondary purpose of also emitting the finished signal.
		 */
		inline void Done() {
			delete this;
		}

		Logger logger = Logger::GetLogger("ExtractionWorker");

	signals:
		void LogMessage(QString message, LogSeverity type = LogSeverity::Info);
		void Finished();
	};

}
}