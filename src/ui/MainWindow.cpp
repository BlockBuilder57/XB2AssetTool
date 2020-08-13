#include "MainWindow.h"
#include "ExtractionWorker.h"

#include <QToolTip>
#include <QScrollBar>
#include <QTextStream>
#include <QFileDialog>

#include "version.h"

namespace xb2at {
namespace ui {

	MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent) {
		ui.setupUi(this);
		setFixedSize(width(), height());
		setWindowTitle(tr("%1 %2").arg(windowTitle(), QString::fromLatin1(version::tag)));

		// replace {VERSION} in the about Markdown
		// with the version tag once
		// TODO maybe make this a lambda so we can replace other keys?
		auto text = ui.aboutxb2at->text();
		text.replace("{VERSION}", QString::fromLatin1(version::tag));
		ui.aboutxb2at->setText(text);

		// connect UI events
		connect(ui.inputBrowse, SIGNAL(clicked()), this, SLOT(InputBrowseButtonClicked()));
		connect(ui.outputBrowse, SIGNAL(clicked()), this, SLOT(OutputBrowseButtonClicked()));
		connect(ui.extractButton, SIGNAL(clicked()), this, SLOT(ExtractButtonClicked()));
		connect(ui.saveLog, SIGNAL(clicked()), this, SLOT(SaveLogButtonClicked()));
		connect(ui.clearLog, SIGNAL(clicked()), this, SLOT(ClearLogButtonClicked()));

		connect(ui.aboutQtButton, SIGNAL(clicked()), this, SLOT(AboutButtonClicked()));

		connect(ui.inputFiles, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged()));
		connect(ui.outputDir, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged()));
	}

	MainWindow::~MainWindow() {
		// do any cleanup here

		if (extraction_thread) {
			delete extraction_thread;
		}
	}

	void MainWindow::InputBrowseButtonClicked() {
		QFileDialog fileSelector(this, "Select input filename", "", "All files (*.*)");
		fileSelector.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
		fileSelector.setFileMode(QFileDialog::FileMode::ExistingFile);

		if (fileSelector.exec()) {

			if(fileSelector.selectedFiles().isEmpty())
				return;

			std::string file = fileSelector.selectedFiles()[0].toStdString();

			QString normalized = QString::fromStdString(file.substr(0, file.find_last_of('.'))).replace('/', fs::path::preferred_separator);

			ui.inputFiles->setText(normalized);
			ui.outputDir->setText(normalized);
			file.clear();
		}
	}

	void MainWindow::OutputBrowseButtonClicked() {
		QFileDialog fileSelector(this, "Select output directory");
		fileSelector.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
		fileSelector.setFileMode(QFileDialog::FileMode::Directory);
		fileSelector.setOption(QFileDialog::ShowDirsOnly);

		if(fileSelector.exec()) {
			if(fileSelector.selectedFiles().isEmpty())
				return;

			QString dir = fileSelector.selectedFiles()[0];

			ui.outputDir->setText(dir.replace('/', fs::path::preferred_separator));
		}
	}

	void MainWindow::TextChanged() {
		// responsively enable/disable extract button
		if(!ui.inputFiles->text().isEmpty() && !ui.outputDir->text().isEmpty()) {
			ui.extractButton->setDisabled(false);
		} else {
			ui.extractButton->setDisabled(true);
		}
	}

	void MainWindow::ExtractButtonClicked() {
		ui.debugConsole->clear();
		ui.extractButton->setDisabled(true);
		ui.allTabs->setCurrentWidget(ui.logTab);

		if(ui.enableVerbose->isChecked())
			Logger::AllowVerbose = true;
		else
			Logger::AllowVerbose = false;
		
		QString file = ui.inputFiles->text();

		ExtractionWorkerOptions options;

		options.saveTextures = ui.saveTextures->isChecked();
		options.saveMorphs = ui.saveMorphs->isChecked();
		options.saveAnimations = ui.saveAnimations->isChecked();
		options.saveOutlines = ui.saveOutlines->isChecked();
		
		switch (ui.formatComboBox->currentIndex()) {
		case 0:
			options.modelFormat = modelSerializerOptions::Format::GLTFBinary;
			break;

		case 1:
			options.modelFormat = modelSerializerOptions::Format::GLTFText;
			break;

		default:
			break;
		}

		options.lod = ui.levelOfDetail->value();

		options.saveMapMesh = ui.saveMapMesh->isChecked();
		options.saveMapProps = ui.saveMapProps->isChecked();

		options.saveXBC1 = ui.saveXbc1->isChecked();

		std::string filename = file.toStdString();

		// Start the thread.
		extraction_thread = new QThread(this);
		ExtractionWorker* et = new ExtractionWorker();
		et->moveToThread(extraction_thread);
		et->setParent(this);

		// connect signals and stuff
		connect(et, SIGNAL(Finished()), this, SLOT(Finished()));
		connect(et, SIGNAL(LogMessage(QString, LogSeverity)), this, SLOT(LogMessage(QString, LogSeverity)));
		connect(extraction_thread, SIGNAL(destroyed()), et, SLOT(deleteLater()));

		// then do it!
		et->DoIt(filename, fs::path(ui.outputDir->text().toStdString()), options);
	}

	void MainWindow::Finished() {
		ui.extractButton->setDisabled(false);
	}


	void MainWindow::ClearLogButtonClicked() {
		ui.debugConsole->clear();
	}

	void MainWindow::SaveLogButtonClicked() {
		QFileDialog savePicker(this, "Select output log path");
		QString rawText = ui.debugConsole->toPlainText();
		rawText.append('\n');

		savePicker.setAcceptMode(QFileDialog::AcceptMode::AcceptSave);
		savePicker.setFileMode(QFileDialog::FileMode::AnyFile);

		if(savePicker.exec()) {
			QStringList selectedFiles = savePicker.selectedFiles();

			if(selectedFiles.isEmpty())
				return;

			QString filename = selectedFiles[0];

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

	void MainWindow::LogMessage(QString message, LogSeverity type) {

		switch (type) {
			default:
			break;
		
			case LogSeverity::Verbose:
				ui.debugConsole->insertHtml("<font>[Verbose] ");
			break;

			case LogSeverity::Info:
				ui.debugConsole->insertHtml("<font>[Info] ");
			break;
		
			case LogSeverity::Warning:
				ui.debugConsole->insertHtml("<font color=\"#b3ae20\">[Warning] ");
			break;

			case LogSeverity::Error:
				ui.debugConsole->insertHtml("<font color=\"#e00d0d\">[Error] ");
			break;
		}

		ui.debugConsole->insertHtml(message);
		ui.debugConsole->insertHtml("</font><br>");
		ui.debugConsole->verticalScrollBar()->setValue(ui.debugConsole->verticalScrollBar()->maximum());

		// process events when signal called
		// (cheap and hacky way of making the application perform better)
		QCoreApplication::processEvents();
	}

	
}
}