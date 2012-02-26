#include "CameraThread.h"

QString		headerPattern("--myboundary\r\nContent-length: (\\d+)\r\nContent-type: image/jpeg\r\n\r\n");

#define REQUEST_DATA_LENGTH		0x74
char requestData[REQUEST_DATA_LENGTH] = {
	0x00, 0x01, 0x00, 0x40, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x21, 0x91, 0x1E,
	0x4C, 0x57, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x20, 0x11, 0x1E, 0x11, 0x23, 0x1F, 0x1E, 0x19,
	0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0xF0, 0x00, 0x1E, 0x00, 0x20, 0x00, 0x21, 0x00, 0x22, 0x00, 0x23, 0x00, 0x25, 0x00, 0x40,
	0x00, 0x44, 0x00, 0x60, 0x00, 0x61, 0x00, 0x80, 0x00, 0x81, 0x00, 0x82, 0x00, 0x83, 0x00, 0xA0,
	0x00, 0xA1, 0x00, 0xA2, 0x00, 0xA3, 0x00, 0xA4, 0x00, 0xA6, 0x00, 0xA7, 0x00, 0xA8, 0x00, 0xA9,
	0x00, 0xAA, 0x00, 0xAD, 0x00, 0xB0, 0x00, 0xB3, 0x00, 0xB4, 0x00, 0xB5, 0x00, 0xB6, 0x00, 0xB7,
	0xFF, 0xFF, 0x00, 0x00};
QByteArray	cameraRequest(requestData, REQUEST_DATA_LENGTH);

CameraThread::CameraThread() : dataLength(-1) {
	this->moveToThread(this);
}
void CameraThread::FillData() {
	QHostInfo hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
	quint32 ipAddress;
	foreach(const QHostAddress &curAddress, hostInfo.addresses()) {
		QString strAddress = "[" + curAddress.toString() + "]";
		ipAddress = curAddress.toIPv4Address();

		if(!strAddress.contains(QRegExp("\\[\\d+\\.\\d+\\.\\d+\\.\\d+\\]")))
			continue;

		break;
	}

	cameraRequest[0x12] = (ipAddress & 0xff000000) >> 24;
	cameraRequest[0x13] = (ipAddress & 0x00ff0000) >> 16;
	cameraRequest[0x14] = (ipAddress & 0x0000ff00) >> 8;
	cameraRequest[0x15] = (ipAddress & 0x000000ff);

	quint16 checkSum = 1;
	for(int i = 0; i < REQUEST_DATA_LENGTH; ++i)
		checkSum += (unsigned char)(cameraRequest.at(i));

	cameraRequest[0x72] = (checkSum & 0xff00) >> 8;
	cameraRequest[0x73] = (checkSum & 0x00ff);
}
void CameraThread::run() {
	FillData();

	QUdpSocket *udpSocketRequest = new QUdpSocket(this);
	udpSocketRequest->bind(10669);

	QByteArray response;
	int requestCounter = 10;
	while(requestCounter--) {
		udpSocketRequest->writeDatagram(cameraRequest, QHostAddress::Broadcast, 10670);

		int counter = 5;
		while(counter) {
			if(udpSocketRequest->hasPendingDatagrams()) {
				response.resize(300);
				int readed = udpSocketRequest->readDatagram(response.data(), 300);
				break;
			}

			sleep(1);
			counter--;
		}

		if(response.size())
			break;
	}

	if(!response.size())
		return;

	QString cameraIP = QString("%1.%2.%3.%4")
								.arg(QString::number(response.at(0x34)))
								.arg(QString::number(response.at(0x35)))
								.arg(QString::number(response.at(0x36)))
								.arg(QString::number(response.at(0x37)));

    QUrl url("http://uname123:password@" + cameraIP + ":80/nphMotionJpeg?Resolution=320x240&Quality=Clarity");

	QNetworkAccessManager *nam = new QNetworkAccessManager(this);
	reply = nam->get(QNetworkRequest(url));

	connect(reply, SIGNAL(readyRead()), this, SLOT(ReadyRead()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
					this, SLOT(Error(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(finished()),
					this, SLOT(Finished()));

	exec();
}
void CameraThread::Error(QNetworkReply::NetworkError error) {
	;
}
void CameraThread::ReadyRead() {
	buffer += reply->readAll();

	QRegExp rx(headerPattern);

	if(-1 == dataLength) {
		int pos;
		if( -1 != (pos = rx.indexIn(buffer)) ) {
			header = rx.cap(0).toAscii();
			dataLength = rx.cap(1).toInt();
			buffer.remove(0, pos + rx.matchedLength());
		}
		else
			return;
	}

	int lengthLeast = (dataLength - data.size());
	if(buffer.size() <= lengthLeast) {
		data += buffer;
		buffer.clear();
	}
	else {
		data += buffer.left(lengthLeast);
		buffer.remove(0, lengthLeast);
	}

	if(data.size() == dataLength) {
		emit SetPictureData(data);
		
		data.clear();
		header.clear();
		dataLength = -1;
	}
}
void CameraThread::Finished() {
	;
}
