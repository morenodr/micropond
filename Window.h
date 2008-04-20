#ifndef WINDOW_H_
#define WINDOW_H_

#include <QtGui>

#include "Renderer.h"
#include "Simulation.h"
#include "CreatureBar.h"

class Window: public QMainWindow
{
	Q_OBJECT
public:
	Window();
	virtual ~Window();
	void initGui();
	void closeEvent ( QCloseEvent * event );
private:
	Simulation *simulation;
	Renderer *renderer;
	CreatureBar *creatureBar;
};

#endif /*WINDOW_H_*/
