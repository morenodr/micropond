#include "Window.h"

Window::Window()
{
	sema = new QSemaphore(1);
	simus = new QList<Simulation *>();
	for(int i = 0; i < THREADS; i++){
		Simulation *temp = new Simulation(i);
		temp->start();
		simus->append(temp);
	}
	
	
	simulation = simus->at(0); //select first thread as default
	renderer = new Renderer(simulation,sema);
	//TODO: create menu entry, and remove. use Window::load
	/*simulation->loadWorld("/home/asraniel/test3");*/
	stat();
}

void Window::stat(){
	unsigned long total = 0;
	for(int i = 0; i < THREADS; i++){
		uint temp = simus->at(i)->executed();
		total += temp;
		qDebug() << "pond" << i << "executed:" << temp;
	}	
	qDebug() << "total executed:" << total;
	QTimer::singleShot(STAT_INTERVAL, this, SLOT(stat()));
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
	viewsGroup->setExclusive(true);
	
	QAction *age = new QAction("Age",viewsGroup);
	connect(age, SIGNAL(triggered()), this, SLOT(ageView()));
	age->setCheckable(true);
	
	QAction *genome = new QAction("Genome",viewsGroup);
	connect(genome, SIGNAL(triggered()), this, SLOT(genomeView()));
	genome->setCheckable(true);
	genome->setChecked(true);
	
	QAction *lineage = new QAction("Lineage",viewsGroup);
	connect(lineage, SIGNAL(triggered()), this, SLOT(lineageView()));
	lineage->setCheckable(true);
		
	QAction *logo = new QAction("Logo",viewsGroup);
	connect(logo, SIGNAL(triggered()), this, SLOT(logoView()));
	logo->setCheckable(true);
			
	QAction *size = new QAction("Size",viewsGroup);
	connect(size, SIGNAL(triggered()), this, SLOT(sizeView()));
	size->setCheckable(true);
	
	QAction *energy = new QAction("Energy",viewsGroup);
	connect(energy, SIGNAL(triggered()), this, SLOT(energyView()));
	energy->setCheckable(true);
	
	QAction *energy2 = new QAction("Energy 2",viewsGroup);
	connect(energy2, SIGNAL(triggered()), this, SLOT(energy2View()));
	energy2->setCheckable(true);
		
	QAction *toxic = new QAction("Toxics",viewsGroup);
	connect(toxic, SIGNAL(triggered()), this, SLOT(toxicView()));
	views->addActions(viewsGroup->actions());
	toxic->setCheckable(true);
	
	pondsGroup = new QActionGroup(this);
	pondsGroup->setExclusive(true);
	connect(pondsGroup , SIGNAL(triggered(QAction *)), this, SLOT(selectPond(QAction *)));
	QMenu *ponds = new QMenu("Ponds");
	for(int i = 0; i < THREADS; i++){
		QAction *pond = new QAction(QString::number(i),pondsGroup);
		pond->setCheckable(true);
		if(i == 0)
			pond->setChecked(true);
	}
	ponds->addActions(pondsGroup->actions());
	
	menuBar->addMenu(file);
	menuBar->addMenu(views);
	menuBar->addMenu(ponds);
	setMenuBar(menuBar);
	
	resize(200,200);
	show();
}

void Window::valueChanged(int val){
	simulation->setEnergyAdd((uint) val);
}

void Window::closeEvent( QCloseEvent * event ){
	closing();
	event->accept();
}

void Window::close(){
	closing();
	//TODO: create menu entry, and remove. use Window::save
	//save("/home/asraniel/test2");
	qApp->quit();
}

void Window::closing(){
	renderer->close();
	for(int i = 0; i < simus->size() ; i++){
		simus->at(i)->resume();
		simus->at(i)->stopIt();
		while(simus->at(i)->isRunning());
	}
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

void Window::selectPond(QAction * pond){
	sema->acquire(1);
	int pondId = pond->text().toInt();
	simulation = simus->at(pondId);
	renderer->setSimulation(simulation);
	
	slider->setRange(0, simulation->getMaxEnergyAdd());
	slider->setValue(simulation->getEnergyAdd());
		
	sema->release(1);
}
