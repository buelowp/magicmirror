/*
    This file is part of MythClock.
    MythClock is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    MythClock is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with MythClock.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __MIRRORFRAME_H__
#define __MIRRORFRAME_H__

#include <QtNetwork/QtNetwork>
#include <QtCore/QtCore>
#include <QtWidgets/QtWidgets>
#include "CalendarData.h"
#include "WeatherData.h"

class MirrorFrame : public QFrame {
	Q_OBJECT
public:
	MirrorFrame(QFrame *parent = 0);
	virtual ~MirrorFrame();

public slots:
	void getEvents();
	void getWeather();
	void calendarEventsDone();
	void calendarEventsError(QString);
	void calendarEventsEvent(QString);
	void weatherDataError(QString);
	void weatherEventsDone();
	void currentConditions(QMap<QString,QString>);
	void localForecast(QString);

//protected:
//	void showEvent(QShowEvent*);

private:
	void deleteCalendarEventsList();

	QTimer *m_calendarTimer;
	QTimer *m_weatherTimer;
	QList<QLabel*> m_calendarEvents;
	QLabel *m_calLabel;
	QLabel *m_currentLabel;
	QLabel *m_forecastLabel;
	int m_calEventsY;
	bool m_newEventList;
};

#endif /* __MIRRORFRAME_H__ */
