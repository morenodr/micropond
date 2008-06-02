#include "Renderer.h"
#include <QtGui>

Renderer::Renderer(Simulation *sim)
{
	simulation = sim;
	colorMode = GENOME;
}
void Renderer::update(){
	updatePicture();
	QTimer::singleShot(UPDATE_INTERVAL, this, SLOT(update()));	
}

QColor Renderer::getColor(struct Cell *cell, int mode){
	int r = 0; //temporary red value
	int g = 0; //temporary green value
	int b = 0; //temporary blue value
	switch(mode){
		case GENERATION:
			if(cell->place->dead && SHOW_LANDSCAPE){
				r = 254;
				g = 254;
				b = 254;
			}else{
				r = qRed(cell->generation*cell->generation);
				g = qGreen(cell->generation*cell->generation);
				b = qBlue(cell->generation*cell->generation);	
			}
			break;
		case GENOME:
			if(cell->place->dead && SHOW_LANDSCAPE){
				r = 254;
				g = 254;
				b = 254;
			}else{
				if(cell->generation >= LIVING_CELL){
					int hash = 0;
					for(uint i = 0; i < cell->genome_size;i++ ){
						if(cell->genome[i] != cell->genome_size-1){
							hash += cell->genome[i];
							hash %= cell->genome_size * 5;
						}else{
							break;
						}
					}
					r = hash  * 900;
					g = hash  * 180;
					b = hash;
				}
			}
			break;
		case LINEAGE:
			if(cell->place->dead && SHOW_LANDSCAPE){
				r = 254;
				g = 254;
				b = 254;
			}else{
				if(cell->generation >= LIVING_CELL){
					r = qRed(cell->lineage * cell->lineage);
					g = qGreen(cell->lineage * cell->lineage);
					b = qBlue(cell->lineage * cell->lineage);
				}
			}
			break;
		case LOGO:
			if(cell->place->dead && SHOW_LANDSCAPE){
				r = 254;
				g = 254;
				b = 254;
			}else{
				if(cell->generation >= LIVING_CELL){
					r = cell->genome[0] * 10;
					g = cell->genome[0] * 10;
					b = cell->genome[0] * 10;
				}
			}
			break;
		case LANDSCAPE:{
			if(cell->place->dead){
				r = 254;
				g = 254;
				b = 254;
			}else{
				r = 0;
				g = 0;
				b = 0;
			}
		}break;
		case ENERGY:
			r = qRed(cell->energy * 900);
			g = qGreen(cell->energy * 180);
			b = qBlue(cell->energy );
			break;
		case ENERGY2:
			r = qRed(cell->energy2 * 900);
			g = qGreen(cell->energy2 * 180);
			b = qBlue(cell->energy2 );
			break;
		case BAD:
			r = qRed(cell->bad * 900);
			g = qGreen(cell->bad * 180);
			b = qBlue(cell->bad * 50);
			break;
	}
	
	return QColor(r % 255,g % 255,b % 255);
}

void Renderer::updatePicture(){	
	int sizeX = simulation->x();
	int sizeY = simulation->y();
	
	QImage temp(sizeX,sizeY,QImage::Format_RGB32);
	simulation->pause();
	for(int x = 0; x < sizeX; x++){
		for(int y = 0; y < sizeY; y++){
			struct Cell *cell = simulation->cell(x,y,0);
			
			temp.setPixel(x,y,getColor(cell,colorMode).rgb());
		}
	}
	int counter = simulation->counter();
	
	simulation->resume();
	setPixmap(QPixmap::fromImage (temp));
	qDebug() << "cells executed: " << counter;
}

void Renderer::changeColorMode(int mode){
	colorMode = mode;
	updatePicture();
}

void Renderer::mousePressEvent ( QMouseEvent * event ){
	if(event->button() == Qt::RightButton){
		changeColorMode((colorMode + 1) % RENDERMODES);		
	}else if(event->button() == Qt::LeftButton){
		simulation->pause();
		struct Cell *cell = simulation->cell(event->x(),event->y(),0);
		qDebug() << "try"<<event->x() << event->y();
		if(cell->generation >= LIVING_CELL){
			qDebug() << "got"<<event->x() << event->y();
			struct Cell tempCell = *cell;
			emit cellSelected(tempCell);
		}	
		
		simulation->resume();
	}
}

Renderer::~Renderer()
{
}
