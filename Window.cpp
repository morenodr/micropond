#include "Window.h"

Window::Window()
{
	simulation = new Simulation();
	renderer = new Renderer(simulation);
	simulation->start();
}

void Window::initGui(){
	setCentralWidget(renderer);
	renderer->update();
	
	creatureBar = new CreatureBar();
	creatureBar->setFeatures(QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea,creatureBar);
	resize(200,200);
	show();
}

void Window::closeEvent ( QCloseEvent * event ){
	renderer->close();
	delete renderer;
	simulation->stopIt();
	while(simulation->isRunning());
	delete simulation;
	event->accept();
}

Window::~Window()
{
}
