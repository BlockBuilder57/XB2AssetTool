#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTextEdit>
#include <QMutex>

#include "ui_xb2at.h"

namespace xb2at {
namespace ui {

	class MainWindow : public QMainWindow {

		Q_OBJECT

	public:
		MainWindow(QWidget *parent = Q_NULLPTR);

		~MainWindow();

	private slots:
		void InputBrowseButtonClicked();
		void OutputBrowseButtonClicked();
		void TextChanged();
		void ExtractButtonClicked();
		void SaveLogButtonClicked();
		// brings up the About Qt thing...
		void AboutButtonClicked();

	private:
		void ExtractFile(std::string filename);

		// Type of message to log
		enum class LogType : std::uint8_t {
			Verbose,
			Info,
			Warning,
			Error
		};

		// Log a message to the log tab
		void LogMessage(QString message, LogType type = LogType::Info);


		// Logging buffer
		QString logBuffer;

		Ui::mainWindow ui;
	};

}
}