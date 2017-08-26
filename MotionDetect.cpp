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
	QString gpio = QString("/sys/class/gpio/gpio%1/value").arg(pin);

	m_pin = new QFile(gpio);
	m_pin->open(QIODevice::ReadOnly | QIODevice::Text);
	if (!m_pin->isOpen()) {
		qWarning() << __PRETTY_FUNCTION__ << ": Unable to open" << gpio;
		return false;
	}
	m_notification = new QSocketNotifier(m_pin->handle(), QSocketNotifier::Read);
	connect(m_notification, SIGNAL(activated()), this, SLOT(detected()));
//	m_notification->setEnabled(false);
	return true;
}

void MotionDetect::close()
{
	m_notification->setEnabled(false);
	m_pin->close();
}

void MotionDetect::detected()
{
	QByteArray value = m_pin->readAll();
//	m_notification->setEnabled(false);
	if (value.size() == 1 && value[0] == '1')
		emit motionDetected();
}

void MotionDetect::enable()
{
//	m_notification->setEnabled(true);
}
