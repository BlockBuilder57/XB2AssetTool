#include "xb2at_ui.h"
#include <QTextStream>
#include <QFileDialog>
#include <thread>

#include <core.h>
#include <msrd_reader.h>
#include <xbc1_reader.h>

using namespace xb2at;

namespace xb2at {
namespace ui {

	MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent) {
		ui.setupUi(this);

		LogMessage("Hello World!", LogType::Info);
	
		LogMessage("Warning test", LogType::Warning);
		LogMessage("Error test", LogType::Error);

		setFixedSize(width(), height());

		// connect UI events
		connect(ui.inputBrowse, SIGNAL(clicked()), this, SLOT(InputBrowseButtonClicked()));
		connect(ui.outputBrowse, SIGNAL(clicked()), this, SLOT(OutputBrowseButtonClicked()));
		connect(ui.extractButton, SIGNAL(clicked()), this, SLOT(ExtractButtonClicked()));
		connect(ui.saveLog, SIGNAL(clicked()), this, SLOT(SaveLogButtonClicked()));

		connect(ui.aboutQtButton, SIGNAL(clicked()), this, SLOT(AboutButtonClicked()));

		connect(ui.inputFiles, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged()));
		connect(ui.outputDir, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged()));
	}


	MainWindow::~MainWindow() {
		if(!logBuffer.isEmpty())
			logBuffer.clear();
	}

	void MainWindow::InputBrowseButtonClicked() {
		QFileDialog fileSelector(this, "Select input files", "", "MSMD Files (*.wismt)");
		QStringList files;
		fileSelector.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
		fileSelector.setFileMode(QFileDialog::FileMode::ExistingFiles);

		if (fileSelector.exec())
			files = fileSelector.selectedFiles();

		ui.inputFiles->setText(files.join(','));
	}

	void MainWindow::OutputBrowseButtonClicked() {
		QFileDialog fileSelector(this, "Select output directory");
		fileSelector.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
		fileSelector.setFileMode(QFileDialog::FileMode::DirectoryOnly);

		if (fileSelector.exec())
			ui.outputDir->setText(fileSelector.selectedFiles()[0]);
	}

	// we use this to responsively disable or enable the extraction button
	void MainWindow::TextChanged() {
		if(!ui.inputFiles->text().isEmpty() && !ui.outputDir->text().isEmpty()) {
			ui.extractButton->setDisabled(false);
		} else {
			ui.extractButton->setDisabled(true);
		}
	}


	void MainWindow::ExtractButtonClicked() {
		ui.allTabs->setCurrentWidget(ui.logTab);
		
		QStringList files = ui.inputFiles->text().split(',');

		for(QString file : files) {
				ExtractFile(file.toStdString());
		}
	}

	void MainWindow::ExtractFile(std::string filename) {
			std::ifstream stream(filename, std::ifstream::binary);
			core::msrdReader reader(stream);
			LogMessage(tr("Extracting file %1...").arg(QString::fromStdString(filename)), LogType::Info);

			reader.set_progress([&](const std::string& progress, core::base_reader::ProgressType type) {
				if(type == core::base_reader::ProgressType::Info)
					LogMessage(tr("[Extraction] %1").arg(QString::fromStdString(progress)), LogType::Info);
				else if(type == core::base_reader::ProgressType::Warning)
					LogMessage(tr("[Extraction] %1").arg(QString::fromStdString(progress)), LogType::Warning);
				else if(type == core::base_reader::ProgressType::Error)
					LogMessage(tr("[Extraction] %1").arg(QString::fromStdString(progress)), LogType::Error);
				else if(type == core::base_reader::ProgressType::Verbose)
					LogMessage(tr("[Extraction] %1").arg(QString::fromStdString(progress)), LogType::Verbose);
			});

			core::msrdReaderOptions opts = { ui.outputDir->text().toStdString(), ui.saveXbc1->isChecked() };
			core::msrd::msrd msrd = reader.Read(opts);
			stream.close();
	}

	void MainWindow::SaveLogButtonClicked() {
		QFileDialog savePicker(this, "Select output log path");
		QString rawText = ui.debugConsole->toPlainText();
		rawText.append('\n');

		// even LITERAL notepad supports Unix line endings
		// but i still wanna do this
#ifdef _WIN32
		rawText.replace('\n', "\r\n");
#endif

		savePicker.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
		savePicker.setFileMode(QFileDialog::FileMode::AnyFile);

		if(savePicker.exec()) {

			// Only using Qt OS abstraction objects here cause this is UI code.
			QString filename = savePicker.selectedFiles()[0];

			if(!filename.isEmpty()) {
				QFile file(filename);

			if(!file.open(QFile::ReadWrite)) {
				rawText.clear();
				return;
			}

			QTextStream stream(&file);

			stream << rawText;
			file.close();
				rawText.clear();
			}
		}

	}

	void MainWindow::AboutButtonClicked() {
		QApplication::aboutQt();
	}

	void MainWindow::LogMessage(QString message, LogType type) {
		if (message.isEmpty())
			return;

		if(type == LogType::Verbose && !ui.enableVerbose->isChecked())
			return;

		switch (type) {
			default:
			break;
		
			case LogType::Verbose:
				logBuffer.append("<font>[Verbose] ");
			break;

			case LogType::Info:
				logBuffer.append("<font>[Info] ");
			break;
		
			case LogType::Warning:
				logBuffer.append("<font color=\"#b3ae20\">[Warning] ");
			break;

			case LogType::Error:
				logBuffer.append("<font color=\"#e00d0d\">[Error] ");
			break;
		}

		logBuffer.append(message);
		logBuffer.append("</font><br>");
		ui.debugConsole->setHtml(logBuffer);
	}

}
}