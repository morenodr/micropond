#ifndef RENDERER_H_
#define RENDERER_H_

#include <QtGui>
#include "Simulation.h"

#define UPDATE_INTERVAL 1000

#define GENERATION 0
#define GENOME 1
#define LINEAGE 2
#define LOGO 3
#define ENERGY 4

#define RENDERMODES 5

#define LIVING_CELL 8

class Renderer: public QLabel
{
	Q_OBJECT
public:
	Renderer(Simulation *sim);
	virtual ~Renderer();
	
	void mousePressEvent ( QMouseEvent * event );
	static QColor getColor(struct Cell *cell, int mode);
	
signals:
	void cellSelected(struct Cell cell);
	
public slots:
	void update();
	
private:
	void updatePicture();
	Simulation *simulation;
	int colorMode;
};

#endif /*RENDERER_H_*/
