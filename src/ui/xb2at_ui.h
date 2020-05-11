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
#include <serializers/mesh_serializer.h>


namespace xb2at {
namespace ui {

	using namespace xb2at::core;

	class MainWindow : public QMainWindow {

		Q_OBJECT

	public:
		MainWindow(QWidget *parent = Q_NULLPTR);

		~MainWindow();

		// TODO: document slots
	private slots:
		void InputBrowseButtonClicked();

		void OutputBrowseButtonClicked();

		void TextChanged();

		void ExtractButtonClicked();

		void SaveLogButtonClicked();
		
		/**
		 * brings up the About Qt thing...
		 */
		void AboutButtonClicked();

	private:

		void ExtractFile(std::string filename, fs::path outputPath, bool saveXbc, meshSerializerOptions::Format meshFormat);

		/**
		 * Extraction thread
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
		 * Mutex for locking exclusive access to the log queue
		 */
		std::mutex log_queue_lock;

		/**
		 * Queue of log messages from reader thread
		 */
		std::queue<ProgressData> log_queue;

		/**
		 * The timer object that handles 
		 * emptying the queue.
		 */
		QTimer* queue_empty_timer;

		/**
		 * Function that runs on the interval.
		 */
		void OnQueueEmptyTimer();

		/**
		 * Log type.
		 */
		enum class LogType : std::uint8_t {
			Verbose,
			Info,
			Warning,
			Error
		};

		/**
		 * Log a message to the "Log" tab in the User Interface, and the logging buffer.
		 * This method is only safe in the UI thread.
		 *
		 * \param[in] message The message to log.
		 * \param[in] type The type of the message.
		 */
		void LogMessage(QString message, LogType type = LogType::Info);

		/**
		 * Logging buffer.
		 */
		QString logBuffer;


		Ui::mainWindow ui;
	};

}
}