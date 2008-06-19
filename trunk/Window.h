#ifndef WINDOW_H_
#define WINDOW_H_

#include <QtGui>

#include "Renderer.h"
#include "Simulation.h"
#include "CreatureBar.h"

#define THREADS 2
#define STAT_INTERVAL 1000

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
	void load(QString file);
	void save(QString file);
	void selectPond(QAction * pond);
	void stat();
	
private:
	void closing();
	
	Simulation *simulation;
	Renderer *renderer;
	CreatureBar *creatureBar;
	QSlider *slider;
	QActionGroup *viewsGroup;
	QActionGroup *pondsGroup;
	QSemaphore *sema; //controls the rendering and loading
	QList <Simulation *>*simus;
	QQueue <struct Cell>*genepool;
	QSemaphore *genepoolblocker;
};

#endif /*WINDOW_H_*/