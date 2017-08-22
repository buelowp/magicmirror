#include "MirrorFrame.h"

MirrorFrame::MirrorFrame(QFrame *parent) : QFrame(parent)
{
	m_calendarTimer = new QTimer();
	connect(m_calendarTimer, SIGNAL(timeout()), this, SLOT(getEvents()));
	m_calendarTimer->start(1000 * 60 * 60);

	m_weatherTimer = new QTimer();
	connect(m_weatherTimer, SIGNAL(timeout()), this, SLOT(getWeather()));
	m_weatherTimer->start(1000 * 60 * 60 * 6);

	QPalette pal(QColor(0,0,0));
	setBackgroundRole(QPalette::Window);
	pal.setColor(QPalette::Window, Qt::black);
	setAutoFillBackground(true);
	setPalette(pal);

	m_calLabel = new QLabel("<font color='white'>Upcoming Events", this);
	m_forecastLabel = new QLabel("<font color='white'>Local Forecast</font>", this);
	m_currentLabel = new QLabel("<font color='white'>Current Conditions</font>", this);

	QFont f("Roboto");
	f.setPixelSize(50);
	f.setBold(true);
	m_calLabel->setGeometry(50, 10, 750, 100);
	m_calLabel->setFont(f);
	m_currentLabel->setGeometry(50, 500, 750, 100);
	m_currentLabel->setFont(f);
	m_forecastLabel->setGeometry(50, 1400, 750, 100);
	m_forecastLabel->setFont(f);
	m_calEventsY = 110;
	m_newEventList = false;
}

MirrorFrame::~MirrorFrame()
{
}

void MirrorFrame::getWeather()
{
	QThread* thread = new QThread;
	WeatherData *event = new WeatherData();
	event->addZip("60005");
	event->addAppID("1f87fe9d8a437f713c617f962df4f0a9");
	event->moveToThread(thread);
	connect(event, SIGNAL(finished()), thread, SLOT(quit()));
	connect(event, SIGNAL(error(QString)), this, SLOT(weatherDataError(QString)));
	connect(event, SIGNAL(currentConditions(QString)), this, SLOT(currentConditions(QString)));
	connect(event, SIGNAL(localForecast(QString)), this, SLOT(localForecast(QString)));
	connect(event, SIGNAL(finished()), event, SLOT(deleteLater()));
	connect(event, SIGNAL(finished()), this, SLOT(weatherEventsDone()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(started()), event, SLOT(process()));
	thread->start();
}

void MirrorFrame::weatherEventsDone()
{
	qDebug() << __PRETTY_FUNCTION__;
}

void MirrorFrame::currentConditions(QMap<QString,QString> conditions)
{
	QMapIterator<QString,QString> i(conditions);

	while (i.hasNext()) {
		i.next();
		qDebug() << __PRETTY_FUNCTION__ << ":" << i.key() << ":" << i.value();
	}
}

void MirrorFrame::localForecast(QString)
{
	qDebug() << __PRETTY_FUNCTION__;
}

void MirrorFrame::weatherDataError(QString)
{
	qDebug() << __PRETTY_FUNCTION__;
}

void MirrorFrame::getEvents()
{
	QThread* thread = new QThread;
	CalendarData *event = new CalendarData();
	event->moveToThread(thread);
	connect(event, SIGNAL(finished()), thread, SLOT(quit()));
	connect(event, SIGNAL(error(QString)), this, SLOT(calendarEventsError(QString)));
	connect(event, SIGNAL(newEvent(QString)), this, SLOT(calendarEventsEvent(QString)));
	connect(event, SIGNAL(finished()), event, SLOT(deleteLater()));
	connect(event, SIGNAL(finished()), this, SLOT(calendarEventsDone()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(started()), event, SLOT(process()));
	thread->start();
}

void MirrorFrame::calendarEventsError(QString)
{
	qDebug() << __PRETTY_FUNCTION__ << ": Error trying to talk to the calendar server";
}

void MirrorFrame::calendarEventsDone()
{
	m_newEventList = true;
	update();
}

void MirrorFrame::calendarEventsEvent(QString s)
{
/*
	if (m_newEventList)
		deleteCalendarEventsList();

	QLabel *lb = new QLabel(s, this);
	QFont f("Roboto");
	f.setPixelSize(25);
	lb->setFont(f);
	lb->setGeometry(100, m_calEventsY + 50, 1000, 50);
	lb->show();
	m_calEventsY += 50;
	m_calendarEvents.push_back(lb);
*/
}

void MirrorFrame::deleteCalendarEventsList()
{
	qDebug() << __PRETTY_FUNCTION__ << ": clearing out list";
	for (int i = 0; i < m_calendarEvents.size(); i++) {
		QLabel *lb = m_calendarEvents.at(i);
		lb->hide();
		delete lb;
	}
	m_calendarEvents.clear();
	m_newEventList = false;
	m_calEventsY = 110;
}
