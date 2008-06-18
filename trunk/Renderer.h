#ifndef RENDERER_H_
#define RENDERER_H_

#include <QtGui>
#include "Simulation.h"

#define UPDATE_INTERVAL 1000

#define GENERATION 0
#define GENOME 1
#define LINEAGE 2
#define LOGO 3
#define LANDSCAPE 4
#define ENERGY 5
#define ENERGY2 6
#define BAD 7

#define RENDERMODES 8

#define LIVING_CELL 3

#define SHOW_LANDSCAPE true

class Renderer: public QLabel
{
	Q_OBJECT
public:
	Renderer(Simulation *sim,QSemaphore *sem);
	virtual ~Renderer();
	
	void mousePressEvent ( QMouseEvent * event );
	static QColor getColor(struct Cell *cell, int mode);
	void setSimulation(Simulation *sim){
		simulation->resume();
		simulation = sim;
	}
	
signals:
	void cellSelected(struct Cell cell);
	
public slots:
	void update();
	void changeColorMode(int mode);
	
private:
	void updatePicture();
	Simulation *simulation;
	int colorMode;
	QSemaphore *sema; //controls the rendering and loading
};

#endif /*RENDERER_H_*/
