#include "CreatureBar.h"
#include "Renderer.h"

CreatureBar::CreatureBar()
{
	QVBoxLayout *layout = new QVBoxLayout;
	QWidget *dockWidget = new QWidget;
	dockWidget->setLayout(layout);
	
	picture = new QLabel();
	
	QPixmap pix( PREVIEW_SIZE, PREVIEW_SIZE);
	pix.fill(Qt::white);
	
	picture->setPixmap(pix);
	
	layout->addWidget(picture);

	id = new QLabel("ID: ");
	layout->addWidget(id);
	
	generation = new QLabel("Generation: ");
	layout->addWidget(generation);
	
	lineage = new QLabel("Lineage: ");
	layout->addWidget(lineage);
	
	genome = new QTextEdit();
	genome->setReadOnly(true);
	layout->addWidget(genome);
	
	//layout->addStretch(1);
	
	setWidget(dockWidget);
	setMinimumWidth(250);
}

QString CreatureBar::operationName(uchar operation){
		
	switch(operation){
	case 0:
		return "reset";
		break;
	case 1: //pointer ++
		return "pointer--";
		break;
	case 2: //pointer --
		return "pointer++";
		break;
	case 3: //register ++
		return "register++";
		break;
	case 4: //register --
		return "register--";
		break;
	case 5: //read genome to register
		return "read genome";
		break;
	case 6: //write register to outputbuffer
		return "write to buffer";
		break;
	case 7: //read output buffer to register
		return "read buffer";
		break;
	case 8: //look into direction specified in the register
		return "face to register";
		break;
	case 9://while(register){
		return "while(register){";
		break;
	case 10://}
		return "}";
		break;
	case 11:
		return "register = direction with most energy";
		break;
	case 12: //move
		return "move to facing";
		break;
	case 13: // kill
		return "kill facing cell";
		break;
	case 14://nop
		return "NOP";
		break;
	case 15://share
		return "share";
		break;
	case 16://swap temp
		return "swap";
		break;
	case 17://reset registers
		return "reset registers";
		break;
	case 18: //probe
		return "probe";
		break;
	case 19: //end
		return "register = neighbour type";
		break;
	case 20: //end
		return "toggle cell activation";
		break;
	case 21: //end
		return "read neighbour activation state";
		break;
	case 22: //end
		return "NOP & NOREP";
		break;
	case 23:
		return "eat";
		break;
	case 24: //end
		return "stop";
		break;
	}
	return "NOT DEFINED"+QString::number(operation);
}

void CreatureBar::cellSelected(struct Cell cell){
	QPixmap pix( PREVIEW_SIZE, PREVIEW_SIZE);
	pix.fill(Renderer::getColor(&cell, 1));
	picture->setPixmap(pix);
	
	generation->setText("Generation: "+QString::number(cell.generation));
	id->setText("ID: "+QString::number(cell.id));
	lineage->setText("Lineage: "+QString::number(cell.lineage));
	genome->clear();
	for(uint i = 0; i < cell.genome_size; i++){
		genome->append(operationName(cell.genome[i]));
	}
	genome->moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
}

CreatureBar::~CreatureBar()
{
}
