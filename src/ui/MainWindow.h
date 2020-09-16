#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QThread>

#include <core.h>

#include "ui_xb2at.h"

// we don't want the kludge here....
#include <modeco/Logger.h>
#undef error
#undef verbose

namespace xb2at {
	namespace ui {

		using namespace xb2at::core;

		class MainWindow : public QMainWindow {
			Q_OBJECT

		   public:
			MainWindow(QWidget* parent = nullptr);

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
			 * Simply clears the console buffer..
			 */
			void ClearLogButtonClicked();

			/**
			 * Fired when the "Save Log" button in the Log tab is clicked.
			 *
			 * Brings up a file selector to select filename and location of the log to save,
			 * and then saves the log in a plain text form, ready for anyone to look at.
			 */
			void SaveLogButtonClicked();

			/**
			 * Fired when the "About Qt..." button is clicked in the About tab.
			 *
			 * Brings up the About Qt dialog.
			 */
			void AboutButtonClicked();

			// Slots other people connect to.
		   public slots:

			/**
			 * Logs a message to the "Log" tab in the User Interface when fired.
			 *
			 * \param[in] message The message to log.
			 * \param[in] type The type/serverity of the message.
			 */
			void LogMessage(QString message, mco::LogSeverity type = mco::LogSeverity::Info);

			/**
			 * When fired, anything needed to be called when extraction is called.
			 */
			void Finished();

		   private:
			/**
			 * Extraction thread.
			 */
			QThread* extraction_thread;

			Ui::mainWindow ui;
		};

	} // namespace ui
} // namespace xb2at