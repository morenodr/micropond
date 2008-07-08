#include "NetworkConfig.h"
#include "Incoming.h"

NetworkConfig::NetworkConfig(Outgoing *outgoing)
{
	out = outgoing;
	
	QWidget *listWidget = new QWidget;
	setWindowTitle("Configure remote ponds");
	
	QGridLayout *layout2 = new QGridLayout(); //main widget
	
	QPushButton *ok = new QPushButton("Ok");
	connect(ok, SIGNAL(clicked()), this, SLOT(save()));
	QPushButton *add = new QPushButton("Add");
	connect(add, SIGNAL(clicked()), this, SLOT(add()));
	
	host = new QLineEdit();
	port = new QSpinBox();
	port->setMaximum(65554);
	port->setValue(PORT);
	
	QListWidget *list = new QListWidget(this);
	
	layout2->addWidget(host,0,0);
	layout2->addWidget(port,0,1);
	layout2->addWidget(add,0,2);
	
	layout2->addWidget(list,1,0,1,3);
	
	for(int i = 0; i < out->size(); i++){
		struct s_host tmpHost = out->getHost(i);
		list->addItem(tmpHost.name +" : " + QString::number(tmpHost.port));
	}
	
	layout2->addWidget(ok,2,2);
	setLayout(layout2);
	resize(listWidget->width() + 50,300);
}

NetworkConfig::~NetworkConfig()
{
}

void NetworkConfig::add(){
	out->addHost(host->text() , port->value());
}


void NetworkConfig::save(){
	accept();
}
