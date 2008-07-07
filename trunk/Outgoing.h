#ifndef OUTGOING_H_
#define OUTGOING_H_

#include <QtNetwork>

#include "Simulation.h"

#define INTERVAL 10000

struct s_host{
	QString name;
	quint16 port;
};

class Outgoing: public QObject
{
	Q_OBJECT
public:
	Outgoing(QQueue <struct Cell>*pool,QSemaphore *geneblocker);
	virtual ~Outgoing();
	
public slots:
	void transfer();
	void deleteHost(int index);
	void addHost(QString host, quint16 port);
	struct s_host getHost(int index);
	
private:
	QQueue <struct Cell>*genepool;
	QSemaphore *genepoolblocker;
	QList <struct s_host>*hosts;
};

#endif /*OUTGOING_H_*/
