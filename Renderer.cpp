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
					r = qRed(cell->generation * 30);
					g = qGreen(cell->generation  * 30);
					b = qBlue(cell->generation  * 30);
					break;
				case GENOME:
					if(cell->generation > 2){
						for(int i = 0; i < simulation->genomeSize();i++ ){
							r += cell->genome[i]%5;
							g += cell->genome[i]%7;
							b += cell->genome[i]%9;
						}
					}
					break;
				case LINEAGE:
					r = qRed(cell->lineage * 200);
					g = qGreen(cell->lineage  * 200);
					b = qBlue(cell->lineage  * 200);
					break;
				case ENERGY:
					r = qRed(cell->energy );
					g = qGreen(cell->energy);
					b = qBlue(cell->energy );
					break;
			}
			//printCell(x,y,0);
			temp.setPixel(x,y,qRgb(r % 256, g % 256, b % 256));
		}
	}
	simulation->resume();
	setPixmap(QPixmap::fromImage (temp));
	//qDebug() << "max generation = " << maxGeneration;
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
		case 1: //pointer --
			qDebug() << "pointer--";
			break;
		case 2: //pointer ++
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
		case 15: //end
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
			printCell(event->x(),event->y(),0);
			printReadableGenome(event->x(),event->y(),0);
		}
		simulation->resume();
	}
}

Renderer::~Renderer()
{
}
