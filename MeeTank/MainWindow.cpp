#include "MainWindow.h"

#include <QMessageBox>

#define LED_RADIUS		30

MainWindow::MainWindow() :
	isTankConnected(false), redLed(":/redLed"), greenLed(":/greenLed")
{
	redLed = redLed.scaled(LED_RADIUS * 2, LED_RADIUS * 2, 
		Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	greenLed = greenLed.scaled(LED_RADIUS * 2, LED_RADIUS * 2, 
		Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	QWidget *w = new QWidget(this);
	QGridLayout *lay = new QGridLayout(w);
	this->setCentralWidget(w);

	Joypad *joypad = new Joypad(this);
	connect(joypad, SIGNAL(SendValue(const QByteArray&)), this, SIGNAL(SendCommand(const QByteArray&)));

	lay->addWidget(joypad,	1, 1, 1, 1);
	lay->setRowStretch(0, 1);
	lay->setColumnStretch(0, 1);

	cameraThread = new CameraThread;
	connect(cameraThread, SIGNAL(SetPictureData(const QByteArray&)),
		this, SLOT(SetPicture(const QByteArray&)));

	cameraThread->start();

	arduinoThread = new ArduinoThread;
	connect(arduinoThread, SIGNAL(SocketConnected()), this, SLOT(SocketConnected()));
	connect(arduinoThread, SIGNAL(SocketDisconnected()), this, SLOT(SocketDisconnected()));
	connect(this, SIGNAL(SendCommand(const QByteArray&)),
		arduinoThread, SLOT(SendCommand(const QByteArray&)));

	arduinoThread->start();
}
MainWindow::~MainWindow() {
	cameraThread->exit(0);
	cameraThread->wait();
	cameraThread->deleteLater();

	arduinoThread->exit(0);
	arduinoThread->wait();
	arduinoThread->deleteLater();
}
void MainWindow::SetPicture(const QByteArray &data) {
	QPixmap pix;
	if(!pix.loadFromData(data))
		return;

	int curWidth = this->width();
	int curHeight = this->height();

    pixmap = pix.scaledToWidth(curWidth);

	this->repaint();
}
void MainWindow::paintEvent(QPaintEvent*) {
	QPainter painter(this);

	int yStart = (this->height() - pixmap.height()) / 2;

	painter.drawPixmap(0, yStart, pixmap);
	painter.drawPixmap(this->width() - redLed.width() - 5, 5, isTankConnected ? greenLed : redLed);
}
void MainWindow::SocketConnected() {
	isTankConnected = true;
	this->repaint();
}
void MainWindow::SocketDisconnected() {
	isTankConnected = false;
	this->repaint();
}
