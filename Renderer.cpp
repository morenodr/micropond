#include "Renderer.h"
#include <QtGui>

Renderer::Renderer(Simulation *sim)
{
	simulation = sim;
	colorMode = GENOME;
}

void Renderer::updatePicture(){
	int sizeX = simulation->x();
	int sizeY = simulation->y();
	int r = 0; //temporary red value
	int g = 0; //temporary green value
	int b = 0; //temporary blue value
	
	uint maxGeneration = 0;
	
	QImage temp(sizeX,sizeY,QImage::Format_RGB32);
	simulation->pause();
	for(int x = 0; x < sizeX; x++){
		for(int y = 0; y < sizeY; y++){
			r = 0;
			g = 0;
			b = 0;
			struct Cell *cell = simulation->cell(x,y,0);
			if(maxGeneration < cell->generation){
				maxGeneration = cell->generation;
			}
			switch(colorMode){
				case GENERATION:
					r = cell->generation;
					g = cell->generation;
					b = cell->generation;
					break;
				case LINEAGE:
					r = cell->lineage % 31;
					g = cell->lineage % 27;
					b = cell->lineage % 13;
					break;
				case GENOME:
					if(cell->generation){
						for(int i = 0; i < simulation->genomeSize();i++ ){
							r += cell->genome[i]%5;
							g += cell->genome[i]%7;
							b += cell->genome[i]%9;
						}
					}
					break;
			}

			temp.setPixel(x,y,qRgb(r % 256, g % 256, b % 256));
		}
	}
	simulation->resume();
	setPixmap(QPixmap::fromImage (temp));
	qDebug() << "max generation = " << maxGeneration;
	QTimer::singleShot(UPDATE_INTERVAL, this, SLOT(updatePicture()));
}


Renderer::~Renderer()
{
}
