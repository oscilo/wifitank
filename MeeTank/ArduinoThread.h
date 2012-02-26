#ifndef ARDUINOTHREAD_H
#define ARDUINOTHREAD_H

#include "globals.h"

class ArduinoThread : public QThread {
	Q_OBJECT

public:
	ArduinoThread();

	void run();

signals:
	void SocketConnected();
    void SocketDisconnected();

public slots:
	void SendCommand(const QByteArray&);

private slots:
	void Disconnected();
	void NewConnection();

private:
	QTcpSocket *sock;
	QTcpServer *serv;
};

#endif
