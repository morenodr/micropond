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
