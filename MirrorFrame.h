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
#include "MotionDetect.h"

#define MONITOR_TIMEOUT		(1000 * 60 * 1)
#define CALEVENTS_TIMEOUT	(1000 * 60 * 60 * 1)
#define FORECAST_TIMEOUT	(1000 * 60 * 60 * 12)
#define CURRENT_TIMEOUT		(1000 * 60 * 60 * 1)

class MirrorFrame : public QFrame {
	Q_OBJECT
public:
	MirrorFrame(QFrame *parent = 0);
	virtual ~MirrorFrame();

public slots:
	void getEvents();
	void getForecast();
	void getCurrentWeather();
	void calendarEventsDone();
	void calendarEventsError(QString);
	void calendarEventsEvent(QString);
	void weatherDataError(QString);
	void weatherEventsDone();
	void currentHumidity(double);
	void currentSkyConditions(QString);
	void currentTemperature(double);
	void currentWindSpeed(double);
	void sunrise(qint64);
	void sunset(qint64);
	void forecastEntry(QJsonObject);
	void forecastFinished();
	void currentConditionsFinished();
	void updateClock();
	void monitorOn();
	void monitorOff();

signals:
	void enableMotionDetect();

//protected:
//	void showEvent(QShowEvent*);

private:
	void deleteCalendarEventsList();
	void createStateMachine();
	void enableTimers();
	void turnMonitorOn();
	void turnMonitorOff();

	QStateMachine *m_monitorState;
	QTimer *m_calendarTimer;
	QTimer *m_forecastTimer;
	QTimer *m_currentWeatherTimer;
	QTimer *m_clockTimer;
	QTimer *m_monitorTimer;
	QList<QLabel*> m_calendarEvents;
	QLabel *m_calLabel;
	QLabel *m_currentLabel;
	QLabel *m_forecastLabel;
	QLabel *m_currentTemp;
	QLabel *m_currentHumidity;
	QLabel *m_currentWind;
	QLabel *m_currentSky;
	QLabel *m_sunrise;
	QLabel *m_sunset;
	QLabel *m_currentTempLabel;
	QLabel *m_currentHumidityLabel;
	QLabel *m_currentWindLabel;
	QLabel *m_currentSkyLabel;
	QLabel *m_sunriseLabel;
	QLabel *m_sunsetLabel;
	QLabel *m_clockLabel;
	QVector<QLabel*> m_forecastEntries;
	
	MotionDetect *m_motionDetect;
	
	int m_calEventsY;
	int m_forecastIndex;
	bool m_newEventList;
	bool m_resetForecastTimer;
};

#endif /* __MIRRORFRAME_H__ */
