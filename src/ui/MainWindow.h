#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QThread>

#include <thread>
#include <mutex>
#include <deque>
#include <queue>

#include "ui_xb2at.h"

#include <core.h>

// Core reader API
#include <readers/msrd_reader.h>
#include <readers/mesh_reader.h>
#include <readers/mxmd_reader.h>
#include <readers/lbim_reader.h>
#include <readers/sar1_reader.h>
#include <readers/skel_reader.h>
#include <serializers/model_serializer.h>


namespace xb2at {
namespace ui {

	using namespace xb2at::core;

	struct uiOptions {
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

		// Perform extraction.
		void DoIt(std::string& filename, fs::path& outputPath, uiOptions& options);

		/**
		 * Wrapper to LogMessage
		 */
		inline void Log(QString message, LogSeverity type = LogSeverity::Info) {
			emit LogMessage(message, type);
		}

		/**
		 * This is a very bad hack to avoid memory leaking.
		 * We delete ourselves when we're done so that we also emit the finished signal.
		 */
		inline void Done() {
			delete this;
		}

	signals:
		void LogMessage(QString message, LogSeverity type = LogSeverity::Info);
		void Finished();
	};



	class MainWindow : public QMainWindow {

		Q_OBJECT

	public:
		MainWindow(QWidget *parent = Q_NULLPTR);

		~MainWindow();

	private slots:
		/**
		 * Fired when the Browse button for the input file is clicked.
		 *
		 * Used to bring up a selection dialog for the user to select the input file.
		 */
		void InputBrowseButtonClicked();

		/**
		 * Fired when the Browse button for the output folder is clicked.
		 *
		 * Used to bring up a selection dialog for the user to select the output directory.
		 */
		void OutputBrowseButtonClicked();

		/**
		 * Fired when the input or output text boxes are modified.
		 *
		 * Used to responsively disable or enable the Extract button.
		 */
		void TextChanged();

		/**
		 * Fired when the Extract button is clicked.
		 *
		 * Starts the extraction procedure.
		 */
		void ExtractButtonClicked();

		/**
		 * Fired when the "Clear Log" button in the Log tab is clicked.
		 *
		 * Simply calls MainWindow::ClearLog().
		 */
		void ClearLogButtonClicked();

		/**
		 * Fired when the "Save Log" button in the Log tab is clicked.
		 *
		 * Brings up a file selector to select filename and location of the log to save,
		 * and then saves the log.
		 */
		void SaveLogButtonClicked();
		
		/**
		 * Fired when the "About Qt..." button is clicked.
		 *
		 * Brings up the About Qt dialog.
		 */
		void AboutButtonClicked();

		// Slots for the public
	public slots:

		/**
		 * Logs a message to the "Log" tab in the User Interface when fired.
		 *
		 * \param[in] message The message to log.
		 * \param[in] type The type/serverity of the message.
		 */
		void LogMessage(QString message, LogSeverity type = LogSeverity::Info);

		/**
		 * Fired
		 */
		void Finished();

	private:

		/**
		 * Extraction thread.
		 */
		QThread* extraction_thread;

		Ui::mainWindow ui;
	};

}
}