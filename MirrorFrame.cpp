#include "MirrorFrame.h"

MirrorFrame::MirrorFrame(QFrame *parent) : QFrame(parent)
{
	m_timer = new QTimer();
	connect(m_timer, SIGNAL(timeout()), this, SLOT(getEvents()));
	m_timer->start(1000 * 60 * 60);
}

MirrorFrame::~MirrorFrame()
{
}

void MirrorFrame::getEvents()
{
	qDebug() << __PRETTY_FUNCTION__ << ":" << __LINE__;
	QThread* thread = new QThread;
	CalendarData *event = new CalendarData();
	event->moveToThread(thread);
	connect(event, SIGNAL(finished()), thread, SLOT(quit()));
	connect(event, SIGNAL(error(QString)), this, SLOT(calendarEventsError(QString)));
	connect(event, SIGNAL(newEvent(QString)), this, SLOT(calendarEventsEvent(QString)));
	connect(event, SIGNAL(finished()), event, SLOT(deleteLater()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(started()), event, SLOT(process()));
	thread->start();
}

void MirrorFrame::calendarEventsDone()
{
	qDebug() << __PRETTY_FUNCTION__ << ": Finished getting events";
}

void MirrorFrame::calendarEventsError(QString)
{
	qDebug() << __PRETTY_FUNCTION__ << ": Error trying to talk to the calendar server";
}

void MirrorFrame::calendarEventsEvent(QString s)
{
 	qDebug() << __PRETTY_FUNCTION__ << ":" << s;
}
