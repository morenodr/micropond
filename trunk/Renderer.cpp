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
			r = qRed(cell->generation);
			g = qGreen(cell->generation);
			b = qBlue(cell->generation);
			break;
		case GENOME:
			if(cell->generation > 2){
				int hash = 0;
				for(uint i = 0; i < cell->genome_size;i++ ){
					if(cell->genome[i] != cell->genome_size-1){
						hash += cell->genome[i];
					}else{
						break;
					}
				}
				r = hash % cell->genome_size + 40;
				g = (hash + 64) % cell->genome_size + 50;
				b = (hash +21) % cell->genome_size + 20;
			}
			break;
		case LINEAGE:
			if(cell->generation > 2){
				r = qRed(cell->lineage);
				g = qGreen(cell->lineage);
				b = qBlue(cell->lineage);
			}
			break;
		case LOGO:
			if(cell->generation > 2){
				r = cell->genome[0] * 10;
				g = cell->genome[0] * 10;
				b = cell->genome[0] * 10;
			}
			break;
		case ENERGY:
			r = qRed(cell->energy * 900);
			g = qGreen(cell->energy * 180);
			b = qBlue(cell->energy );
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

void Renderer::printCell(int x, int y, int z){
	struct Cell *cell = simulation->cell(x,y,0);
	QString genome("");
	
	int stop = 0;
	if(cell->generation > 2){
		for(int i = 0; i < simulation->genomeSize();i++ ){
			genome.append(QString::number(cell->genome[i])+" ");
			if(cell->genome[i] == 15){
				stop++;
				if(stop >= 4){
					break;
				}
			}
		}
		qDebug() << "Pos" << x << y << z << "Gene" << cell->generation << "genome"<< genome;
	}
}

void Renderer::printReadableGenome(int x, int y, int z){
	struct Cell *cell = simulation->cell(x,y,z);
	int stop = 0;
	
	for(int i = 0; i < simulation->genomeSize();i++ ){
		switch(cell->genome[i]){
		case 0:
			qDebug() << "reset";
			break;
		case 1: //pointer ++
			qDebug() << "pointer--";
			break;
		case 2: //pointer --
			qDebug() << "pointer++";
			break;
		case 3: //register ++
			qDebug() << "register++";
			break;
		case 4: //register --
			qDebug() << "register--";
			break;
		case 5: //read genome to register
			qDebug() << "read genome";
			break;
		case 6: //write register to outputbuffer
			qDebug() << "write to buffer";
			break;
		case 7: //read output buffer to register
			qDebug() << "read buffer";
			break;
		case 8: //look into direction specified in the register
			qDebug() << "face to register";
			break;
		case 9://while(register){
			qDebug() << "while(register){";
			break;
		case 10://}
			qDebug() << "}";
			break;
		case 11:
			qDebug() << "register = direction with most energy";
			break;
		case 12: //move
			qDebug() << "move to facing";
			break;
		case 13: // kill
			qDebug() << "kill facing cell";
			break;
		case 14://nop
			qDebug() << "NOP";
			break;
		case 15://share
			qDebug() << "share";
			break;
		case 16://swap temp
			qDebug() << "swap";
			break;
		case 17://reset registers
			qDebug() << "reset registers";
			break;
		case 18: //probe
			qDebug() << "probe";
			break;
		case 19: //end
			qDebug() << "stop";
			break;
		}
		if(cell->genome[i] == 15){
			stop++;
			if(stop >= 4){
				break;
			}
		}
	}
}

void Renderer::mousePressEvent ( QMouseEvent * event ){
	if(event->button() == Qt::RightButton){
		colorMode = (colorMode + 1) % RENDERMODES;
		updatePicture();
	}else if(event->button() == Qt::LeftButton){
		simulation->pause();
		struct Cell *cell = simulation->cell(event->x(),event->y(),0);
		if(cell->generation > 2){
			/*printCell(event->x(),event->y(),0);
			printReadableGenome(event->x(),event->y(),0);*/
			struct Cell tempCell = *cell;
			emit cellSelected(tempCell);
		}	
		
		simulation->resume();
	}
}

Renderer::~Renderer()
{
}
