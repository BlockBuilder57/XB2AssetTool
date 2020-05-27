#include "xb2at_ui.h"
#include <QTextStream>
#include <QFileDialog>
#include <thread>

#include "version.h"

namespace xb2at {
namespace ui {

	MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent) {
		ui.setupUi(this);
		setFixedSize(width(), height());
		setWindowTitle(tr("%1 %2").arg(windowTitle(), QString::fromLatin1(version::tag)));

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
		QFileDialog fileSelector(this, "Select input filename", "", "All files (*.*)");
		fileSelector.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
		fileSelector.setFileMode(QFileDialog::FileMode::ExistingFile);

		if (fileSelector.exec()) {

			if(fileSelector.selectedFiles().isEmpty())
				return;

			std::string file = fileSelector.selectedFiles()[0].toStdString();

			QString normalized = QString::fromStdString(file.substr(0, file.find_last_of('.'))).replace('/', fs::path::preferred_separator);

			ui.inputFiles->setText(normalized);
			file.clear();
		}
	}

	void MainWindow::OutputBrowseButtonClicked() {
		QFileDialog fileSelector(this, "Select output directory");
		fileSelector.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
		fileSelector.setFileMode(QFileDialog::FileMode::DirectoryOnly);

		if (fileSelector.exec()) {
			if(fileSelector.selectedFiles().isEmpty())
				return;

			QString dir = fileSelector.selectedFiles()[0];

			ui.outputDir->setText(dir.replace('/', fs::path::preferred_separator));
		}
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
		ui.extractButton->setDisabled(true);
		ui.allTabs->setCurrentWidget(ui.logTab);
		
		QString file = ui.inputFiles->text();

		
		queue_empty_timer = new QTimer(this);
		queue_empty_timer->callOnTimeout(std::bind(&MainWindow::OnQueueEmptyTimer, this));
		queue_empty_timer->start(10);

		// TODO: make mesh format a dropdown thing
		extraction_thread = std::thread(std::bind(&MainWindow::ExtractFile, this, file.toStdString(), fs::path(ui.outputDir->text().toStdString()), ui.saveXbc1->isChecked(), modelSerializerOptions::Format::GLTFText));
		extraction_thread.detach();
	}

	void MainWindow::OnQueueEmptyTimer() {
		// Spin this function until we are able to 
		// lock the mutex guarding the log message queue.
		std::lock_guard<std::mutex> lock(log_queue_lock);

		// Process the queue until it is empty.
		while(log_queue.size() != 0) {
			auto message = log_queue.front();

			// Cast the progress type to LogType.
			// ProgressType and LogType have the same order so we do not lose type safety
			LogMessage(QString::fromStdString(message.data), (LogType)message.type);

			// If we recieved a finish message,
			// do what we need to do to uninitalize the things we allocated in ExtractButtonClicked() 
			if(message.finished) {
				// Stop ourselves.
				if (queue_empty_timer)
					queue_empty_timer->stop();

				// Delete the timer object.
				// ExtractButtonClicked() creates a new timer object each time it is invoked,
				// so we need to delete the current instance.
				if (queue_empty_timer)
					delete queue_empty_timer;

				// Enable the "Extract" button in the UI.
				ui.extractButton->setDisabled(false);
			}

			log_queue.pop();
		}

	}

	void MainWindow::ProgressFunction(const std::string& progress, ProgressType type, bool finish = false) {
		// Spinlock until the calling thread can lock the mutex
		std::lock_guard<std::mutex> lock(log_queue_lock);

		log_queue.push({progress, type, finish});
	}

	void MainWindow::ExtractFile(std::string filename, fs::path& outputPath, bool saveXbc, modelSerializerOptions::Format meshFormat) {
			using namespace std::placeholders;
			ProgressFunction(tr("Extracting %1...").arg(QString::fromStdString(filename)).toStdString(), ProgressType::Info);
			
			fs::path path(filename);

			if(!fs::exists(outputPath)) {
				ProgressFunction("Creating output path as it didn't exist", ProgressType::Warning);
				fs::create_directories(outputPath);
			}

			path.replace_extension(".wismt");

			if(fs::exists(path)) {
				std::ifstream stream(path.string(), std::ifstream::binary);
				auto func = std::bind(&MainWindow::ProgressFunction, this, _1, _2, false);
				msrdReader msrdreader(stream);
				modelSerializer ms;

				msrd::msrd msrd;
				mesh::mesh mesh;
				mxmd::mxmd mxmd;
				
				msrdreader.set_progress(func);
				msrd = msrdreader.Read({outputPath, saveXbc});

				// Close the wismt file and set the extension
				// in preparation of reading the wimdo/MXMD
				stream.close();
				path.replace_extension(".wimdo");

				for(int i = 0; i < msrd.files.size(); ++i) {
					if(msrd.dataItems[i].type == msrd::data_item_type::Model) {
						meshReader meshreader;
						
						ProgressFunction("File " + std::to_string(i) + " is a mesh", ProgressType::Verbose);

						meshreader.forward(msrdreader);

						ProgressFunction("Reading mesh " + std::to_string(i), ProgressType::Info);
						mesh = meshreader.Read({msrd.files[i].data});

						if(mesh.dataSize != 0)
							msrd.meshes.push_back(mesh);
					}
				}

				if (fs::exists(path)) {
					stream.open(path.string(), std::ifstream::binary);
					mxmdReader mxmdreader(stream);

					mxmdreader.forward(msrdreader);
					mxmd = mxmdreader.Read({});

					stream.close();
				} else {
					ProgressFunction(path.stem().string() + ".wimdo doesn't exist.", ProgressType::Error, true);
					return;
				}

				ms.forward(msrdreader);
				ms.Serialize(msrd.meshes, { meshFormat, mxmd, outputPath, path.stem().string()});
			} else {
				ProgressFunction(path.stem().string() + ".wismt doesn't exist.", ProgressType::Error, true);
				return;
			}

			// Signal finish
			ProgressFunction("Extraction completed.", ProgressType::Info, true);
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

			// Only using Qt OS abstraction objects here cause this is UI code.
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