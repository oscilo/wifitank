#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "globals.h"

#include "CameraThread.h"
#include "ArduinoThread.h"

#include "Joypad.h"

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();

signals:
    void SendCommand(const QByteArray&);

private slots:
	void SetPicture(const QByteArray&);
	void SocketConnected();
    void SocketDisconnected();

protected:
	void paintEvent(QPaintEvent*);

private:
	CameraThread *cameraThread;
	ArduinoThread *arduinoThread;
	QLabel *tankConnectionLabel;

	bool isTankConnected;
	QPixmap pixmap;
	QPixmap redLed;
	QPixmap greenLed;
};

#endif
