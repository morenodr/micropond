#ifndef RENDERER_H_
#define RENDERER_H_

#include <QtGui>
#include "Simulation.h"

#define UPDATE_INTERVAL 1000

#define GENERATION 0
#define GENOME 1
#define LINEAGE 2

class Renderer: public QLabel
{
	Q_OBJECT
public:
	Renderer(Simulation *sim);
	virtual ~Renderer();
	
public slots:
	void updatePicture();
	
private:
	Simulation *simulation;
	int colorMode;
};

#endif /*RENDERER_H_*/
