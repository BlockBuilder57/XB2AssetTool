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


	class ExtractionThread : public QObject {
		Q_OBJECT

	public:

		~ExtractionThread() {
			emit Finished();
		}


		void DoIt(std::string& filename, fs::path& outputPath, uiOptions& options);

		// simple wrapper to emit LogMessage
		void Log(QString message, ProgressType type = ProgressType::Info) {
			emit LogMessage(message, type);
		}

		// hack (but we're always allocated with new)
		inline void Done() {
			delete this;
		}

	signals:
		void LogMessage(QString message, ProgressType type = ProgressType::Info);
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

		// public slots
	public slots:

		/**
		 * Log a message to the "Log" tab in the User Interface, and the logging buffer.
		 *
		 * \param[in] message The message to log.
		 * \param[in] type The type of the message.
		 */
		void LogMessage(QString message, ProgressType type = ProgressType::Info);

		void Finished();

	private:

		/**
		 * Extraction thread.
		 */
		QThread* extraction_thread;


		/**
		 * Clear the information in the "Log" tab in the User Interface.
		 */
		void ClearLog();


		Ui::mainWindow ui;
	};

}
}