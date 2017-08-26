/*
 * MotionDetect.h
 *
 *  Created on: Aug 25, 2017
 *      Author: pete
 */

#ifndef MOTIONDETECT_H_
#define MOTIONDETECT_H_

#include <QtCore/QtCore>

class MotionDetect: public QObject {
	Q_OBJECT
public:
	MotionDetect(QObject *parent = 0);
	virtual ~MotionDetect();

	bool open(int);
	void close();

signals:
	void motionDetected();

public slots:
	void detected();
	void enable();

private:
	QFile *m_pin;
	QSocketNotifier *m_notification;
};

#endif /* MOTIONDETECT_H_ */
