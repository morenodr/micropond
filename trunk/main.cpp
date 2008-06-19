#include <QtCore>
#include <QCoreApplication>

#include "Window.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
		
	int threads = THREADS;
	if(argc > 1){
		threads = QString(argv[1]).toInt();
	}
    Window *gui = new Window(threads);
    gui->initGui();
    return a.exec();
}
