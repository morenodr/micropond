#include "Outgoing.h"

Outgoing::Outgoing(QQueue <struct Cell>*pool,QSemaphore *geneblocker)
{
	genepool = pool;
	genepoolblocker = geneblocker;
	
	hosts = new QList <struct s_host>();
	
	addHost("127.0.0.1",5443);
	
	QTimer::singleShot(INTERVAL, this, SLOT(transfer()));
}


Outgoing::~Outgoing()
{
}


void Outgoing::transfer(){
	QTcpSocket *socket = new QTcpSocket();
	
	if(!hosts->size())
		return;
		
	int target = qrand() % hosts->size();
	
	socket->connectToHost(hosts->at(target).name, hosts->at(target).port, QIODevice::ReadOnly);
	
	qDebug() << "starting transfer";
	
	if(socket->waitForReadyRead(2000)){
		qDebug() << "hmm";
		quint32 number = 55;
		QDataStream in(socket);
		in >> number;
		
		qDebug() << "receiving" << number << "cells";
		
		for(uint i = 0; i < number; i++){
			struct Cell cell; 
			
			int read = in.readRawData((char *)&cell,sizeof(cell));
			if(read == sizeof(cell)){
				qDebug() << "got cell" << cell.id << cell.generation;
							
				genepoolblocker->acquire(1);
				genepool->enqueue(cell);
				genepoolblocker->release(1);
			}
		}
	}else{
		qDebug() << "could not connect to distant pond";
	}
	
	socket->close();
	
	QTimer::singleShot(INTERVAL, this, SLOT(transfer()));
}

void Outgoing::deleteHost(int index){
	hosts->removeAt(index);
}

void Outgoing::addHost(QString host, quint16 port){
	struct s_host newHost;
	newHost.name = host;
	newHost.port = port;
	hosts->append(newHost);
}

struct s_host Outgoing::getHost(int index){
	return hosts->at(index);
}

