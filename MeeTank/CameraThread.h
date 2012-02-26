#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include "globals.h"

class CameraThread : public QThread {
	Q_OBJECT

public:
	CameraThread();

	void run();

signals:
	void SetPictureData(const QByteArray&);
	
private slots:
	void ReadyRead();
	void Error(QNetworkReply::NetworkError);
	void Finished();

private:
	void FillData();

	QNetworkReply *reply;
	QByteArray buffer;
	QByteArray header;
	QByteArray data;
	int dataLength;
};

#endif