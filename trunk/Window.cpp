#include "Window.h"

Window::Window()
{
	sema = new QSemaphore(1);
	simulation = new Simulation();
	renderer = new Renderer(simulation,sema);
	//TODO: create menu entry, and remove. use Window::load
	simulation->loadWorld("/home/asraniel/test3");
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
	
	
	//MENU
	QMenuBar *menuBar = new QMenuBar();
	QMenu *file = new QMenu("File");
	QAction *close = new QAction("close",this);
	connect(close , SIGNAL(triggered()), this, SLOT(close()));
	file->addAction(close);
	
	QMenu *views = new QMenu("Viewmode");
	
	viewsGroup = new QActionGroup(this);
	
	QAction *age = new QAction("Age",viewsGroup);
	connect(age, SIGNAL(triggered()), this, SLOT(ageView()));
	views->addAction(age);
	age->setChecked(true);
	
	QAction *genome = new QAction("Genome",viewsGroup);
	connect(genome, SIGNAL(triggered()), this, SLOT(genomeView()));
	views->addAction(genome);
	
	QAction *lineage = new QAction("Lineage",viewsGroup);
	connect(lineage, SIGNAL(triggered()), this, SLOT(lineageView()));
	views->addAction(lineage);
		
	QAction *logo = new QAction("Logo",viewsGroup);
	connect(logo, SIGNAL(triggered()), this, SLOT(logoView()));
	views->addAction(logo);
			
	QAction *size = new QAction("Size",viewsGroup);
	connect(size, SIGNAL(triggered()), this, SLOT(sizeView()));
	views->addAction(size);
	
	QAction *energy = new QAction("Energy",viewsGroup);
	connect(energy, SIGNAL(triggered()), this, SLOT(energyView()));
	views->addAction(energy);
	
	QAction *energy2 = new QAction("Energy 2",viewsGroup);
	connect(energy2, SIGNAL(triggered()), this, SLOT(energy2View()));
	views->addAction(energy2);
		
	QAction *toxic = new QAction("Toxics",viewsGroup);
	connect(toxic, SIGNAL(triggered()), this, SLOT(toxicView()));
	views->addAction(toxic);
	
	menuBar->addMenu(file);
	menuBar->addMenu(views);
	setMenuBar(menuBar);
	
	resize(200,200);
	show();
}

void Window::valueChanged(int val){
	simulation->setEnergyAdd((uint) val);
}

void Window::closeEvent( QCloseEvent * event ){
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

void Window::ageView(){
	renderer->changeColorMode(0);
}

void Window::genomeView(){
	renderer->changeColorMode(1);
}

void Window::lineageView(){
	renderer->changeColorMode(2);
}

void Window::logoView(){
	renderer->changeColorMode(3);
}

void Window::sizeView(){
	renderer->changeColorMode(4);
}

void Window::energyView(){
	renderer->changeColorMode(5);
}

void Window::energy2View(){
	renderer->changeColorMode(6);
}

void Window::toxicView(){
	renderer->changeColorMode(7);
}

void Window::load(QString file){
	sema->acquire(1);
	simulation->resume();
	simulation->stopIt();
	while(!simulation->isFinished());
	simulation->loadWorld(file);
	simulation->start();
	sema->release(1);
}

void Window::save(QString file){
	sema->acquire(1);
	simulation->resume();
	simulation->stopIt();
	while(!simulation->isFinished());
	simulation->saveWorld(file);
	simulation->start();
	sema->release(1);
}

void Window::close(){
	renderer->close();
	//delete renderer;
	simulation->resume();
	simulation->stopIt();
	//TODO: create menu entry, and remove. use Window::save
	save("/home/asraniel/test2");
	qApp->quit();
}
