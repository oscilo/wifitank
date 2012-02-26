#ifndef JOYPAD_H
#define JOYPAD_H

#include "globals.h"

class Joypad : public QWidget {
	Q_OBJECT

public:
	Joypad(QWidget *parent);

signals:
	void SendValue(const QByteArray&);

protected:
	void mouseMoveEvent(QMouseEvent *e);
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void paintEvent(QPaintEvent *e);

private:
	void SetValues(float, float);

	QPixmap manipulatorPix;
	bool isPressed;
	int xValue;
	int yValue;
};

#endif