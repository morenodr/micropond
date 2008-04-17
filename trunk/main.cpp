#include <QtCore>
#include <QCoreApplication>

#include "Window.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
		
    Window *gui = new Window();
    gui->initGui();
    return a.exec();
}
