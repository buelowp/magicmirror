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
	void detected(int);
	void enable();

private:
	void setEdge(int);
	void setDirection(int);
	bool openValue(int);
	bool openUEvent(int);

	QFile *m_uevent;
	QFile *m_value;
	QSocketNotifier *m_notification;
	char m_detectValue;
};

#endif /* MOTIONDETECT_H_ */
