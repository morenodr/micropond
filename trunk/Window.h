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
	
public slots:
	void valueChanged(int val);
	void ageView();
	void genomeView();
	void lineageView();
	void logoView();
	void sizeView();
	void energyView();
	void energy2View();
	void toxicView();
	void close();
private:
	Simulation *simulation;
	Renderer *renderer;
	CreatureBar *creatureBar;
	QSlider *slider;
	QActionGroup *viewsGroup;
};

#endif /*WINDOW_H_*/
