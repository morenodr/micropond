#ifndef WINDOW_H_
#define WINDOW_H_

#include <QtGui>

#include "Renderer.h"
#include "Simulation.h"

class Window: public QMainWindow
{
	Q_OBJECT
public:
	Window();
	virtual ~Window();
	void initGui();
private:
	Simulation *simulation;
	Renderer *renderer;
};

#endif /*WINDOW_H_*/
