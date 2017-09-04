/*
 * MotionDetect.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: pete
 */

#include "MotionDetect.h"

MotionDetect::MotionDetect(QObject *parent) : QObject(parent)
{
}

MotionDetect::~MotionDetect()
{
}

bool MotionDetect::open(int pin)
{
//	if (!openUEvent(pin))
//		return false;

	if (!openValue(pin))
		return false;

	setEdge(pin);
	setDirection(pin);

	qDebug() << __PRETTY_FUNCTION__ << ": opened gpio" << pin; 
	return true;
}

bool MotionDetect::openUEvent(int pin)
{
	QString uevent = QString("/sys/class/gpio/gpio%1/uevent").arg(pin);

	m_uevent = new QFile(uevent);
	m_uevent->open(QIODevice::ReadOnly | QIODevice::Text);
	if (!m_uevent->isOpen()) {
		qWarning() << __PRETTY_FUNCTION__ << ": Unable to open" << uevent;
		return false;
	}
	
	m_notification = new QSocketNotifier(m_uevent->handle(), QSocketNotifier::Read);
	m_notification->setEnabled(true);
	connect(m_notification, SIGNAL(activated(int)), this, SLOT(detected(int)));
	return true;
}

bool MotionDetect::openValue(int pin)
{
	QString value = QString("/sys/class/gpio/gpio%1/value").arg(pin);
	char state;

	m_value = new QFile(value);
	m_value->open(QIODevice::ReadOnly | QIODevice::Text);
	if (!m_value->isOpen()) {
		qWarning() << __PRETTY_FUNCTION__ << ": Unable to open" << value;
		return false;
	}
	m_notification = new QSocketNotifier(m_value->handle(), QSocketNotifier::Read);
	m_notification->setEnabled(true);
	connect(m_notification, SIGNAL(activated(int)), this, SLOT(detected(int)));
	m_value->read(&state, 1);
	m_value->seek(0);
	qDebug() << __PRETTY_FUNCTION__ << ": opened" << value << "with contents" << state;
	return true;
}

void MotionDetect::setEdge(int pin)
{
	QFile edge(QString("/sys/class/gpio/gpio%1/edge").arg(pin));

	edge.open(QIODevice::WriteOnly | QIODevice::Text);
	if (edge.isOpen()) {
		edge.write("rising");
	}
	edge.close();
}

void MotionDetect::setDirection(int pin)
{
	QFile direction(QString("/sys/class/gpio/gpio%1/direction").arg(pin));

	direction.open(QIODevice::WriteOnly | QIODevice::Text);
	if (direction.isOpen()) {
		direction.write("in");
	}
	direction.close();
}

void MotionDetect::close()
{
	qDebug() << __PRETTY_FUNCTION__;
	m_notification->setEnabled(false);
	m_value->close();
	m_uevent->close();
}

void MotionDetect::detected(int)
{
	char value;

	if (!m_value->isOpen())
		qDebug() << __PRETTY_FUNCTION__ << ": m_value is not open";
	else {
		m_value->read(&value, 1); 
		m_value->seek(0);
		if (m_detectValue != value) {
			m_detectValue = value;
			if (value == '1') {
				qDebug() << __PRETTY_FUNCTION__ << value;
				emit motionDetected();
			}
		}
	}
}

void MotionDetect::enable()
{
	qDebug() << __PRETTY_FUNCTION__;
	m_notification->setEnabled(true);
}
