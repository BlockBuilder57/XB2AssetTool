#include "xb2at_ui.h"
#include <QToolTip>
#include <QScrollBar>
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
		connect(ui.clearLog, SIGNAL(clicked()), this, SLOT(ClearLogButtonClicked()));

		connect(ui.aboutQtButton, SIGNAL(clicked()), this, SLOT(AboutButtonClicked()));

		connect(ui.inputFiles, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged()));
		connect(ui.outputDir, SIGNAL(textChanged(const QString&)), this, SLOT(TextChanged()));
	}

	MainWindow::~MainWindow() {
		ClearLog();
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
		if(!ui.inputFiles->text().isEmpty() && !ui.outputDir->text().isEmpty()) {
			ui.extractButton->setDisabled(false);
		} else {
			ui.extractButton->setDisabled(true);
		}
	}


	void MainWindow::ExtractButtonClicked() {
		ClearLog();
		ui.extractButton->setDisabled(true);
		ui.allTabs->setCurrentWidget(ui.logTab);
		
		QString file = ui.inputFiles->text();

		// Make the timer for the UI to check for log messages
		queue_empty_timer = new QTimer(this);
		queue_empty_timer->callOnTimeout(std::bind(&MainWindow::OnQueueEmptyTimer, this));

		uiOptions options;

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

		// Start the timer and the thread.
		queue_empty_timer->start(10);
		extraction_thread = std::thread(std::bind(&MainWindow::ExtractFile, this, filename, fs::path(ui.outputDir->text().toStdString()), options));
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

	void MainWindow::ExtractFile(std::string& filename, fs::path& outputPath, uiOptions& options) {
			using namespace std::placeholders;

			PROGRESS_UPDATE_MAIN(ProgressType::Info, false, "Input: " << filename)
			PROGRESS_UPDATE_MAIN(ProgressType::Info, false, "Output path: " << outputPath.string())
			
			fs::path path(filename);
			std::string filenameOnly = path.stem().string();

			if(!fs::exists(outputPath)) {
				ProgressFunction("Creating output directory tree", ProgressType::Info);
				fs::create_directories(outputPath);
			}

			if (options.saveTextures)
				if(!fs::exists(outputPath / "Textures"))
					fs::create_directories(outputPath / "Textures");

			if(options.saveXBC1)
				if (!fs::exists(outputPath / "Dump"))
					fs::create_directories(outputPath / "Dump");

			path.replace_extension(".wismt");

			if(fs::exists(path)) {
				std::ifstream stream(path.string(), std::ifstream::binary);
				auto func = std::bind(&MainWindow::ProgressFunction, this, _1, _2, false);

				msrdReaderOptions msrdoptions = { outputPath / "Dump", options.saveXBC1 };
				msrdReader msrdreader(stream);
				modelSerializer ms;

				msrd::msrd msrd;
				mesh::mesh mesh;
				mxmd::mxmd mxmd;
				
				msrdreader.set_progress(func);

				PROGRESS_UPDATE_MAIN(ProgressType::Info, false, "Reading MSRD file...")

				msrd = msrdreader.Read(msrdoptions);

				if(msrdoptions.Result != msrdReaderStatus::Success) {
					PROGRESS_UPDATE_MAIN(ProgressType::Error, true, "Error reading MSRD file: " << msrdReaderStatusToString(msrdoptions.Result))
					return;
				}

				// Close the wismt file and set the extension
				// in preparation of reading the wimdo/MXMD
				stream.close();
				path.replace_extension(".wimdo");

				for(int i = 0; i < msrd.files.size(); ++i) {
					switch(msrd.dataItems[i].type) {
						case msrd::data_item_type::Model: {
							meshReader meshreader;
							meshReaderOptions meshoptions(msrd.files[i].data);
						
							PROGRESS_UPDATE_MAIN(ProgressType::Verbose, false, "MSRD file " << i << "is a mesh")

							meshreader.forward(msrdreader);

							PROGRESS_UPDATE_MAIN(ProgressType::Info, false, "Reading mesh " << i)
							mesh = meshreader.Read(meshoptions);

							if(meshoptions.Result == meshReaderStatus::Success) {
								msrd.meshes.push_back(mesh);
							} else {
								PROGRESS_UPDATE_MAIN(ProgressType::Error, true, "Error reading mesh from MSRD file " << i << ": " << meshReaderStatusToString(meshoptions.Result))
								return;
							}
						} break;

						case msrd::data_item_type::Texture: {
							lbimReader lbimreader;
							lbimReaderOptions lbimoptions(msrd.files[i].data);

							PROGRESS_UPDATE_MAIN(ProgressType::Info, false, "MSRD file " << i << "is a texture")

							lbimreader.forward(msrdreader);
							lbim::texture texture = lbimreader.Read(lbimoptions);

							if(lbimoptions.Result != lbimReaderStatus::Success) {
								PROGRESS_UPDATE_MAIN(ProgressType::Error, true, "Error reading LBIM: " << lbimReaderStatusToString(lbimoptions.Result))
								return;
							}
						} break;

					}
				}

				if (fs::exists(path)) {
					stream.open(path.string(), std::ifstream::binary);
					mxmdReader mxmdreader(stream);
					mxmdReaderOptions mxmdoptions = {};

					mxmdreader.forward(msrdreader);
					mxmd = mxmdreader.Read(mxmdoptions);

					stream.close();

					if (mxmdoptions.Result != mxmdReaderStatus::Success) {
						PROGRESS_UPDATE_MAIN(ProgressType::Error, true, "Error reading MXMD file: " << mxmdReaderStatusToString(mxmdoptions.Result))
						return;
					}

					PROGRESS_UPDATE_MAIN(ProgressType::Info, false, "MXMD file successfully read")
				} else {
					PROGRESS_UPDATE_MAIN(ProgressType::Error, true, filenameOnly << ".wimdo doesn't exist.");
					return;
				}

				ms.forward(msrdreader);
				modelSerializerOptions msoptions = { options.modelFormat, outputPath, filenameOnly, options.lod, options.saveMorphs, options.saveOutlines };
				ms.Serialize(msrd.meshes, mxmd, msoptions);
			} else {
				PROGRESS_UPDATE_MAIN(ProgressType::Error, true, filenameOnly << ".wismt doesn't exist.");
				return;
			}

			// Signal successful finish
			PROGRESS_UPDATE_MAIN(ProgressType::Info, true, "Extraction successful")
	}

	void MainWindow::ClearLogButtonClicked() {
		ClearLog();
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



	void MainWindow::ClearLog() {
		// Clear both the log buffer and the debug console buffer.
		logBuffer.clear();
		ui.debugConsole->clear();
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
		ui.debugConsole->verticalScrollBar()->setValue(ui.debugConsole->verticalScrollBar()->maximum());
	}

}
}