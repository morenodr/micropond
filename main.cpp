#include <QtCore>
#include <QCoreApplication>

#include "Simulation.h"

int main(int argc, char *argv[])
{
	argc = 0;
	argv = 0;
    Simulation *simulation = new Simulation();
    simulation->run();
}
