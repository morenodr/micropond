#ifndef RENDERER_H_
#define RENDERER_H_

#include <QtGui>
#include "Simulation.h"

#define UPDATE_INTERVAL 250

#define GENERATION 0
#define GENOME 1
#define LINEAGE 2
#define ENERGY 3

#define RENDERMODES 4

class Renderer: public QLabel
{
	Q_OBJECT
public:
	Renderer(Simulation *sim);
	virtual ~Renderer();
	
	void mousePressEvent ( QMouseEvent * event );
public slots:
	void update();
	
private:
	void updatePicture();
	void printCell(int x, int y, int z);
	void printReadableGenome(int x, int y, int z);
	Simulation *simulation;
	int colorMode;
};

#endif /*RENDERER_H_*/
