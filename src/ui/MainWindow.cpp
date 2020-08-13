#include "MainWindow.h"
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
		if (message.isEmpty())
			return;

		// new logger has checks for this that's just a single bool check
//		if(type == LogSeverity::Verbose && !ui.enableVerbose->isChecked())
//			return;

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
		// (cheap way of making the application perform better)
		QCoreApplication::processEvents();
	}

	//
	// EXTRACTION THREAD
	//

	void ExtractionWorker::DoIt(std::string& filename, fs::path& outputPath, uiOptions& options) {
			using namespace std::placeholders;

			// marshal from C++ -> QT
			auto progress = [&](std::string message, LogSeverity type) {
					Log(QString::fromStdString(message), type);
			};

			// set logger output function
			// (we can't only set it once because we rely on a non-static wrapper,
			// and then we would have an invalid `this` pointer once this function exits. Sucks, I know.)
			Logger::OutputFunction = progress;
			
			logger.info("Input: ", filename);
			logger.info("Output path: ", outputPath.string());
			
			fs::path path(filename);
			std::string filenameOnly = path.stem().string();

			if(!fs::exists(outputPath)) {
				logger.info("Creating output directory tree");
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

				msrdReaderOptions msrdoptions = { outputPath / "Dump", options.saveXBC1 };
				msrdReader msrdreader(stream);
				modelSerializer ms;

				msrd::msrd msrd;
				mesh::mesh mesh;
				mxmd::mxmd mxmd;
				skel::skel skel;
				
				logger.info("Reading MSRD file.");

				msrd = msrdreader.Read(msrdoptions);

				if(msrdoptions.Result != msrdReaderStatus::Success) {
					//VARARGS_LOG(LogSeverity::Error, "Error reading MSRD file: " << msrdReaderStatusToString(msrdoptions.Result))
					logger.error("Error reading MSRD file: ", msrdReaderStatusToString(msrdoptions.Result));
					Done();
					return;
				}

				auto readSAR1 = [&]() {
					sar1::sar1 sar1;

					stream.open(path.string(), std::ifstream::binary);
					sar1Reader sar1reader(stream);
					sar1ReaderOptions sar1options = {};

					sar1 = sar1reader.Read(sar1options);

					stream.close();

					if (sar1options.Result != sar1ReaderStatus::Success) {
						logger.error("Error reading SAR1 file: ", sar1ReaderStatusToString(sar1options.Result));
						return sar1;
					}

					return sar1;
				};

				auto readSKEL = [&]() {
					skel::skel skel;
					sar1::sar1 sar1_skeleton = readSAR1();

					sar1::bc* bcItem;
					for (int i = 0; i < sar1_skeleton.numFiles; i++)
					{
						if (sar1_skeleton.tocItems[i].filename.find(".skl") != std::string::npos)
							bcItem = &sar1_skeleton.bcItems[i];
					}

					if (bcItem == nullptr) {
						logger.error("Skeleton not found in ", path.filename().string(), ", continuing without skeleton...");
						return skel;
					}

					skelReader skelreader;
					skelReaderOptions skeloptions = {(*bcItem).data};

					logger.info("Reading SKEL in ", path.filename().string());
					skel = skelreader.Read(skeloptions);

					if(skeloptions.Result != skelReaderStatus::Success) {
						logger.error("Error reading skeleton, continuing without skeleton...");
					}

					return skel;
				};

				// Close the wismt file and set the extension
				// in preparation of reading the wimdo/MXMD
				stream.close();

				for(int i = 0; i < msrd.files.size(); ++i) {
					switch(msrd.dataItems[i].type) {
						case msrd::data_item_type::Model: {
							meshReader meshreader;
							meshReaderOptions meshoptions(msrd.files[i].data);
						
							logger.verbose("MSRD file ", i, " is a mesh");
							
							logger.verbose("Reading mesh ", i, "...");
							mesh = meshreader.Read(meshoptions);

							if(meshoptions.Result == meshReaderStatus::Success) {
								msrd.meshes.push_back(mesh);
							} else {
								logger.error("Error reading mesh from MSRD file ", i, ": ", meshReaderStatusToString(meshoptions.Result));
								Done();
								return;
							}
						} break;

						case msrd::data_item_type::Texture: {
							lbimReader lbimreader;
							lbimReaderOptions lbimoptions(msrd.files[1].data, &msrd.files[msrd.dataItems[i].tocIndex == 0 ? 0 : msrd.dataItems[i].tocIndex - 1].data);

							lbimoptions.offset = msrd.dataItems[i].offset;
							lbimoptions.size = msrd.dataItems[i].size;

							logger.verbose("MSRD file ", i, " is a texture");

							lbim::texture texture = lbimreader.Read(lbimoptions);
							texture.filename = msrd.textureNames[i - 2];

							if(lbimoptions.Result != lbimReaderStatus::Success) {
								logger.error("Error reading LBIM: ", lbimReaderStatusToString(lbimoptions.Result));
							} else {
								msrd.textures.push_back(texture);
								logger.info("LBIM ", i, " successfully read.");
							}
						} break;

						case msrd::data_item_type::CachedTextures: {
							lbimReader lbimreader;
							lbimReaderOptions lbimoptions(msrd.files[0].data, nullptr);

							for (int j = 0; j < msrd.textureCount; ++j) {
								lbimoptions.offset = msrd.dataItems[i].offset + msrd.textureInfo[j].offset;
								lbimoptions.size = msrd.textureInfo[j].size;

								logger.verbose("MSRD texture ", j, " has a CachedTexture");

								lbim::texture texture = lbimreader.Read(lbimoptions);
								texture.filename = msrd.textureNames[j];

								if(lbimoptions.Result != lbimReaderStatus::Success) {
									logger.error("Error reading LBIM: ", lbimReaderStatusToString(lbimoptions.Result));
								} else {
									msrd.textures.push_back(texture);
									logger.info("Cached LBIM ", j, " successfully read.");
								}
							}
						} break;

					}
				}

				msrd.files.clear();
				msrd.textureNames.clear();

				path.replace_extension(".arc");
				// note: this is not how skeleton version differences will be implemented,
				// but it's the only method so far of telling the difference between XC2 and XCDE models w/out asking the user
				if (fs::exists(path)) {
					// we're probably an XC2 model 
					skel = readSKEL();
				} else {
					path.replace_extension(".chr");
					if (fs::exists(path)) {
						// we're probably an XCDE model
						skel = readSKEL();
					} else {
						logger.warn("Skeleton doesn't exist, continuing without skeleton...");
					}
				}
				
				path.replace_extension(".wimdo");

				if (fs::exists(path)) {
					stream.open(path.string(), std::ifstream::binary);
					mxmdReader mxmdreader(stream);
					mxmdReaderOptions mxmdoptions = {};

					mxmd = mxmdreader.Read(mxmdoptions);

					stream.close();

					if (mxmdoptions.Result != mxmdReaderStatus::Success) {
						logger.error("Error reading MXMD file: ", mxmdReaderStatusToString(mxmdoptions.Result));
						Done();
						return;
					}

					logger.info("MXMD file successfully read.");
				} else {
					logger.error(filenameOnly, ".wimdo doesn't exist.");
					Done();
					return;
				}

				modelSerializerOptions msoptions = { options.modelFormat, outputPath, filenameOnly, options.lod, options.saveMorphs, options.saveOutlines };
				ms.Serialize(msrd.meshes, mxmd, skel, msoptions);
			
				msrd.meshes.clear();
				msrd.textures.clear();
				msrd.dataItems.clear();
			} else {
				logger.error(filenameOnly, ".wismt doesn't exist.");
				Done();
				return;
			}

			// Signal successful finish
			logger.info("Extraction successful.");
			Done();
	}

}
}