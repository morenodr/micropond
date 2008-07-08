#ifndef NETWORKCONFIG_H_
#define NETWORKCONFIG_H_

#include <QtGui>
#include "Outgoing.h"

class NetworkConfig: public QDialog
{
	Q_OBJECT
public:
	NetworkConfig(Outgoing *outgoing);
	virtual ~NetworkConfig();
	
public slots:
	void save();
	void add();
	
private:
	Outgoing *out;
	QLineEdit *host;
	QSpinBox *port;
};

#endif /*NETWORKCONFIG_H_*/
