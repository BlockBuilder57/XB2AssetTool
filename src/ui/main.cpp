#include "xb2at_ui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QCoreApplication::addLibraryPath("./");
	QApplication a(argc, argv);
	xb2at::ui::MainWindow w;
	w.show();
	return a.exec();
}
