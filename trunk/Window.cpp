#include "Window.h"

Window::Window()
{
	simulation = new Simulation();
	renderer = new Renderer(simulation);
	simulation->start();
}

void Window::initGui(){
	
	renderer->update();
	
	slider = new QSlider();
	slider->setRange(0, simulation->getMaxEnergyAdd());
	slider->setValue(simulation->getEnergyAdd());
	slider->setOrientation(Qt::Horizontal);
	
	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
	
	QWidget *central = new QWidget();
	QBoxLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom);
	layout->addWidget(renderer);
	layout->addWidget(slider);
	central->setLayout(layout);
	
	setCentralWidget(central);
	
	creatureBar = new CreatureBar();
	creatureBar->setFeatures(QDockWidget::DockWidgetMovable);
	
	connect(renderer,SIGNAL(cellSelected(struct Cell)),
			creatureBar,SLOT(cellSelected(struct Cell)));
	
	addDockWidget(Qt::RightDockWidgetArea,creatureBar);
	resize(200,200);
	show();
}

void Window::valueChanged(int val){
	simulation->setEnergyAdd((uint) val);
}

void Window::closeEvent ( QCloseEvent * event ){
	renderer->close();
	//delete renderer;
	simulation->resume();
	simulation->stopIt();
	while(simulation->isRunning());
	//delete simulation;
	event->accept();
}

Window::~Window()
{
}
