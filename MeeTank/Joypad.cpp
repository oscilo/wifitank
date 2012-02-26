#include "Joypad.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define JOYPAD_RADIUS		120
#define MANIPULATOR_RADIUS	50

#define FORWARD				1
#define BACKWARD			2

int maxRadius = JOYPAD_RADIUS - MANIPULATOR_RADIUS;

Joypad::Joypad(QWidget *parent) :
	QWidget(parent),
	isPressed(false),
	xValue(-MANIPULATOR_RADIUS),
	yValue(-MANIPULATOR_RADIUS),
	manipulatorPix(":/manipulator")
{
	this->setFixedSize(JOYPAD_RADIUS*2 + 1, JOYPAD_RADIUS*2 + 1);
	manipulatorPix = manipulatorPix.scaled(MANIPULATOR_RADIUS * 2, MANIPULATOR_RADIUS * 2,
		Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}
void Joypad::mouseMoveEvent(QMouseEvent *e) {
	if(!isPressed)
		return;

	float xCur = e->x() - JOYPAD_RADIUS;
	float yCur = e->y() - JOYPAD_RADIUS;

	float radius = sqrt(xCur*xCur + yCur*yCur);

	if(radius > maxRadius) {
		float k = maxRadius / radius;

		xCur = xCur * k;
		yCur = yCur * k;
	}

	SetValues(xCur, yCur);
}
void Joypad::mousePressEvent(QMouseEvent *e) {
	isPressed = true;
}
void Joypad::mouseReleaseEvent(QMouseEvent *e) {
	isPressed = false;
	SetValues(0, 0);
}
void Joypad::paintEvent(QPaintEvent *e) {
	QPainter painter(this);

	painter.setBrush(QBrush(QColor(0, 0, 0, 150)));
	painter.setPen(Qt::NoPen);

	painter.fillRect(this->rect(), QColor(255, 255, 255, 0));
	painter.drawEllipse(QPoint(JOYPAD_RADIUS, JOYPAD_RADIUS), JOYPAD_RADIUS, JOYPAD_RADIUS);
	painter.drawPixmap(QPoint(JOYPAD_RADIUS + xValue, JOYPAD_RADIUS + yValue),
		manipulatorPix);
}
void Joypad::SetValues(float xCur, float yCur) {
	xValue = xCur - MANIPULATOR_RADIUS;
	yValue = yCur - MANIPULATOR_RADIUS;

	yCur = -yCur;

	int directionLeft;
	int directionRight;
	int speedLeft;
	int speedRight;

	float max = 255.;
	float radius;
	float value;
	
	radius = yCur*yCur + xCur*xCur;

	max *= sqrt(radius) / maxRadius;
	
	if(radius < 0.0001)
		value = 0;
	else
		value = max * (yCur*yCur - xCur*xCur) / radius;

	if(yCur > 0) {
		if(xCur > 0) {
			speedRight = value;
			speedLeft = max;
		}
		else {
			speedRight = max;
			speedLeft = value;
		}
	}
	else {
		if(xCur < 0) {
			speedRight = -value;
			speedLeft = -max;
		}
		else {
			speedRight = -max;
			speedLeft = -value;
		}
	}

    directionRight = (speedRight > 0) ?  BACKWARD : FORWARD;
    directionLeft = (speedLeft > 0) ? BACKWARD : FORWARD;
	speedRight = abs(speedRight);
	speedLeft = abs(speedLeft);

	QString command = QString("[%1%2%3%4]")
						.arg(directionRight)
						.arg(speedRight, 2, 0x10, QChar('0'))
						.arg(directionLeft)
						.arg(speedLeft, 2, 0x10, QChar('0'));

	emit SendValue(command.toAscii());
	
	this->repaint();
}
