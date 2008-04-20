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
	
	layout->addStretch(1);
	
	setWidget(dockWidget);
	setMinimumWidth(150);
}

void CreatureBar::cellSelected(struct Cell cell){
	QPixmap pix( PREVIEW_SIZE, PREVIEW_SIZE);
	pix.fill(Renderer::getColor(&cell, 1));
	picture->setPixmap(pix);
	
	generation->setText("Generation: "+QString::number(cell.generation));
	id->setText("ID: "+QString::number(cell.id));
	lineage->setText("Lineage: "+QString::number(cell.lineage));
}

CreatureBar::~CreatureBar()
{
}
