#include "ArduinoThread.h"

ArduinoThread::ArduinoThread(): 
	sock(0), serv(0)
{
	this->moveToThread(this);
}
void ArduinoThread::run() {
	serv = new QTcpServer(this);
	connect(serv, SIGNAL(newConnection()), this, SLOT(NewConnection()));

    serv->listen(QHostAddress::Any, 34543);

    QProcess proc;
    proc.start("/sbin/ifconfig");
    proc.waitForFinished();
    QString ifconfigOutput = proc.readAllStandardOutput();

    QString strAddress;
    QRegExp rx("addr:([^ ]+)");
    int offset = 0;
    while(-1 != (rx.indexIn(ifconfigOutput, offset))) {
        offset += rx.matchedLength();

        strAddress = "[" + rx.cap(1) + "]";

        if(strAddress == "[127.0.0.1]")
            continue;
		if(!strAddress.contains(QRegExp("\\[\\d+\\.\\d+\\.\\d+\\.\\d+\\]")))
			continue;

		break;
	}

	QUdpSocket *udpSocket = new QUdpSocket(this);
	udpSocket->writeDatagram(strAddress.toAscii(), QHostAddress::Broadcast, 12345);
	
	exec();
}
void ArduinoThread::NewConnection() {
	sock = serv->nextPendingConnection();

	if(!sock)
		return;

	connect(sock, SIGNAL(disconnected()), this, SLOT(Disconnected()));

	emit SocketConnected();
}
void ArduinoThread::Disconnected() {
	sock = 0;

	emit SocketDisconnected();
}
void ArduinoThread::SendCommand(const QByteArray &data) {
	if(!sock)
		return;

	sock->write(data);
	sock->flush();
}
