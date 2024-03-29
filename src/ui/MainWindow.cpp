#include "MainWindow.h"
#include "ExtractionWorker.h"

#include <QToolTip>
#include <QScrollBar>
#include <QTextStream>
#include <QFileDialog>

#include "version.h"

namespace xb2at {
	namespace ui {

		MainWindow::MainWindow(QWidget* parent)
			: QMainWindow(parent) {
			ui.setupUi(this);

			setFixedSize(width(), height());

			// replace a keyword in the about Markdown with a value
			auto ReplaceKeyWord = [=](const QString& identifier, const QString& replacement) {
				// this makes copies which is Bad
				auto text = ui.aboutxb2at->text();
				text.replace(identifier, replacement);
				ui.aboutxb2at->setText(text);
			};

			// TODO: if the described tag contains -N or -N-dirty or the branch isn't master then
			// also show what branch the code was built on.

			setWindowTitle(tr("%1 %2").arg(windowTitle(), QString::fromLatin1(version::tag)));
			ReplaceKeyWord("{VERSION}", QString::fromLatin1(version::tag));

			// connect all of the UI events to functions in here

			connect(ui.inputBrowse, &QPushButton::clicked, this, &MainWindow::InputBrowseButtonClicked);
			connect(ui.outputBrowse, &QPushButton::clicked, this, &MainWindow::OutputBrowseButtonClicked);
			connect(ui.extractButton, &QPushButton::clicked, this, &MainWindow::ExtractButtonClicked);
			connect(ui.saveLog, &QPushButton::clicked, this,&MainWindow::SaveLogButtonClicked);

			connect(ui.clearLog, &QPushButton::clicked, [=](){
				ui.debugConsole->clear();
			});

			connect(ui.aboutQtButton, &QPushButton::clicked, []() {
				QApplication::aboutQt();
			});

			// connect textchanged events to our handler for both of them
			connect(ui.inputFiles, &QLineEdit::textChanged, this, &MainWindow::TextChanged);
			connect(ui.outputDir, &QLineEdit::textChanged, this, &MainWindow::TextChanged);
		}

		MainWindow::~MainWindow() {
			// do any cleanup here

			// if the extraction thread somehow exists remove it
			delete extraction_thread;
		}

		void MainWindow::InputBrowseButtonClicked() {
			QFileDialog fileSelector(this, "Select input filename", "", "All files (*.*)");
			fileSelector.setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
			fileSelector.setFileMode(QFileDialog::FileMode::ExistingFile);

			if(fileSelector.exec()) {
				if(fileSelector.selectedFiles().isEmpty())
					return;

				std::string file = fileSelector.selectedFiles()[0].toStdString();

				// normalize the path by replacing the file seperator with the platform prefered one
				// (Qt seems to always prefer / instead of choosing the platform preferred one)

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

				// Do the same thing as above, but with less scariness.
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

			//mco::Logger::SetAllowVerbose(ui.enableVerbose->isChecked());

			QString file = ui.inputFiles->text();

			ExtractionWorkerOptions options;

			options.saveTextures = ui.saveTextures->isChecked();
			options.saveMorphs = ui.saveMorphs->isChecked();
			options.saveAnimations = ui.saveAnimations->isChecked();
			options.saveOutlines = ui.saveOutlines->isChecked();

			switch(ui.formatComboBox->currentIndex()) {
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

			// Create the thread and extraction objects
			extraction_thread = new QThread(this);
			auto* et = new ExtractionWorker();

			// and move the extraction worker to the thread
			et->moveToThread(extraction_thread);

			// connect all of the signals to the UI slots
			connect(et, SIGNAL(Finished()), this, SLOT(Finished()));
			connect(et, SIGNAL(LogMessage(QString, mco::LogSeverity)), this, SLOT(LogMessage(QString, mco::LogSeverity)));
			connect(extraction_thread, SIGNAL(destroyed()), et, SLOT(deleteLater()));

			fs::path outputPath(ui.outputDir->text().toStdString());

			// then do it!
			et->ExtractAll(filename, outputPath, options);
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

		void MainWindow::LogMessage(QString message, mco::LogSeverity type) {
			QTextCursor cursor(ui.debugConsole->textCursor());
			QTextCharFormat format;

			switch(type) {
				case mco::LogSeverity::Verbose:
				case mco::LogSeverity::Info:
				default:
					break;

				case mco::LogSeverity::Warning:
					format.setForeground(Qt::darkYellow);
					break;

				case mco::LogSeverity::Error:
					format.setForeground(Qt::red);
					break;
			}

			cursor.setCharFormat(format);
			cursor.insertText(message);
			cursor.insertText("\n");

			ui.debugConsole->ensureCursorVisible();

			// process events when signal called
			// (cheap and hacky way of making the application perform better)
			QCoreApplication::processEvents();
		}

	} // namespace ui
} // namespace xb2at