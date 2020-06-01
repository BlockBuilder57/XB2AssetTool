#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QTimer>

#include <thread>
#include <mutex>
#include <deque>
#include <queue>

#include "ui_xb2at.h"

#include <core.h>

#include <readers/msrd_reader.h>
#include <readers/mesh_reader.h>
#include <readers/mxmd_reader.h>
#include <readers/lbim_reader.h>
#include <serializers/model_serializer.h>


namespace xb2at {
namespace ui {

	using namespace xb2at::core;

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

	private:

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
		 * Extract data from files.
		 * Can be used as an example of how to use xb2core to convert files from the file repressentation
		 * to xb2core repressentation with proper error handling.
		 *
		 * \param[in] filename Filename to extract all applicable data from
		 * \param[in] outputPath Output path
		 * \param[in] options All options
		 */
		void ExtractFile(std::string& filename, fs::path& outputPath, uiOptions& options);

		/**
		 * Extraction thread.
		 */
		std::thread extraction_thread;

		/**
		 * Progress function for the UI.
		 * Does not call UI functions, rather, pushes to a queue that is emptied on a timer on the main thread
		 * (as to not make Qt upset)
		 *
		 * \param[in] message Message to push.
		 * \param[in] type Progress message type.
		 * \param[in] finish if work is finished
		 */
		void ProgressFunction(const std::string& message, ProgressType type, bool finish);

		/**
		 * Progress data that we store in a struct.
		 */
		struct ProgressData {
			std::string data;
			core::ProgressType type;
			bool finished;
		};

		/**
		 * Mutex for locking exclusive access to the log queue.
		 */
		std::mutex log_queue_lock;

		/**
		 * Queue of log messages from extraction thread.
		 * 
		 */
		std::queue<ProgressData> log_queue;

		/**
		 * The timer object that handles emptying the log queue.
		 */
		QTimer* queue_empty_timer;

		/**
		 * Function that runs when the timer elapses on the UI thread.
		 * Locks the mutex, empties the log queue (actually posting the messages)
		 * and also handles the timer destruction (to avoid memory leaks).
		 */
		void OnQueueEmptyTimer();

		/**
		 * Log type. Should be synced and in order of ProgressType.
		 */
		enum class LogType : std::uint8_t {
			Verbose,
			Info,
			Warning,
			Error
		};

		/**
		 * Clear both the log buffer and the information in the "Log" tab in the User Interface.
		 */
		void ClearLog();

		/**
		 * Log a message to the "Log" tab in the User Interface, and the logging buffer.
		 * This method is only safe to use on the UI thread.
		 *
		 * \param[in] message The message to log.
		 * \param[in] type The type of the message.
		 */
		void LogMessage(QString message, LogType type = LogType::Info);

		/**
		 * The logging buffer.
		 */
		QString logBuffer;

		Ui::mainWindow ui;
	};

}
}