/*
 * MotionDetect.cpp
 *
 *  Created on: Aug 25, 2017
 *      Author: pete
 */

#include "MotionDetect.h"

MotionDetect::MotionDetect(QObject *parent) : QObject(parent)
{
	m_timer = new QTimer();
	connect(m_timer, SIGNAL(timeout()), this, SLOT(detected()));
	m_timer->start(1000 * 60 * 2);
}

MotionDetect::~MotionDetect()
{
}

bool MotionDetect::open()
{
	return true;
}

void MotionDetect::close()
{
}

void MotionDetect::detected()
{
	emit motionDetected();
}
