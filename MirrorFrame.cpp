#include "MirrorFrame.h"

MirrorFrame::MirrorFrame(QFrame *parent) : QFrame(parent)
{
	m_calendarTimer = new QTimer();
	connect(m_calendarTimer, SIGNAL(timeout()), this, SLOT(getEvents()));
	m_calendarTimer->start(1000 * 60 * 60);

	m_forecastTimer = new QTimer();
	connect(m_forecastTimer, SIGNAL(timeout()), this, SLOT(getForecast()));
	m_forecastTimer->start(1000 * 60 * 60);

	m_currentWeatherTimer = new QTimer();
	connect(m_currentWeatherTimer, SIGNAL(timeout()), this, SLOT(getCurrentWeather()));
	m_currentWeatherTimer->start(1000 * 60 * 60 * 12);

	QPalette pal(QColor(0,0,0));
	setBackgroundRole(QPalette::Window);
	pal.setColor(QPalette::Window, Qt::black);
	setAutoFillBackground(true);
	setPalette(pal);

	m_calLabel = new QLabel("<font color='white'>Upcoming Events", this);
	m_forecastLabel = new QLabel("<font color='white'>Local Forecast</font>", this);
	m_currentLabel = new QLabel("<font color='white'>Outside Conditions</font>", this);

	QFont f("Roboto");
	f.setPixelSize(50);
	f.setBold(true);
	m_calLabel->setGeometry(50, 10, 750, 100);
	m_calLabel->setFont(f);
	m_currentLabel->setGeometry(50, 1000, 750, 100);
	m_currentLabel->setFont(f);
	m_forecastLabel->setGeometry(50, 1400, 750, 100);
	m_forecastLabel->setFont(f);

	f.setPixelSize(30);
	m_currentTempLabel = new QLabel(this);
	m_currentHumidityLabel = new QLabel(this);
	m_currentWindLabel = new QLabel(this);
	m_currentSkyLabel = new QLabel(this);

	m_currentTempLabel = new QLabel(this);
	m_currentTempLabel->setGeometry(100, 1100, 300, 50);
	m_currentTempLabel->setFont(f);
	m_currentTempLabel->setText("<center>Current Temperature</center>");

	m_currentHumidityLabel = new QLabel(this);
	m_currentHumidityLabel->setGeometry(400, 1100, 300, 50);
	m_currentHumidityLabel->setFont(f);
	m_currentHumidityLabel->setText("<center>Current Humidity</center>");

	m_sunriseLabel = new QLabel(this);
	m_sunriseLabel->setGeometry(700, 1100, 300, 50);
	m_sunriseLabel->setFont(f);
	m_sunriseLabel->setText("<center>Sunrise</center>");

	m_currentWindLabel = new QLabel(this);
	m_currentWindLabel->setGeometry(100, 1200, 300, 50);
	m_currentWindLabel->setFont(f);
	m_currentWindLabel->setText("<center>Current Wind Speed</center>");

	m_currentSkyLabel = new QLabel(this);
	m_currentSkyLabel->setGeometry(400, 1200, 300, 50);
	m_currentSkyLabel->setFont(f);
	m_currentSkyLabel->setText("<center>Sky Conditions</center>");

	m_sunsetLabel = new QLabel(this);
	m_sunsetLabel->setGeometry(700, 1200, 300, 50);
	m_sunsetLabel->setFont(f);
	m_sunsetLabel->setText("<center>Sunset</center>");

	f.setPixelSize(25);
	f.setBold(false);

	m_currentTemp = new QLabel(this);
	m_currentTemp->setGeometry(100, 1150, 300, 50);
	m_currentTemp->setFont(f);

	m_currentHumidity = new QLabel(this);
	m_currentHumidity->setGeometry(400, 1150, 300, 50);
	m_currentHumidity->setFont(f);

	m_sunrise = new QLabel(this);
	m_sunrise->setGeometry(700, 1150, 300, 50);
	m_sunrise->setFont(f);

	m_currentWind = new QLabel(this);
	m_currentWind->setGeometry(100, 1250, 300, 50);
	m_currentWind->setFont(f);

	m_currentSky = new QLabel(this);
	m_currentSky->setGeometry(400, 1250, 300, 50);
	m_currentSky->setFont(f);

	m_sunset = new QLabel(this);
	m_sunset->setGeometry(700, 1250, 300, 50);
	m_sunset->setFont(f);

	for (int i = 0; i < 3; i++) {
		QLabel *lb = new QLabel(this);
		lb->setGeometry(100, ((i * 50) + 1500), 1000, 50);
		lb->setFont(f);
		m_forecastEntries.push_back(lb);
	}

	m_calEventsY = 110;
	m_forecastIndex = 0;
	m_newEventList = false;
}

MirrorFrame::~MirrorFrame()
{
}

void MirrorFrame::getCurrentWeather()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MagicMirror", "MagicMirror");

	QThread* thread = new QThread;
	WeatherData *event = new WeatherData();
	event->addZip("60005");
	event->addAppID(settings.value("appid").toString());
	event->moveToThread(thread);
	connect(event, SIGNAL(finished()), thread, SLOT(quit()));
	connect(event, SIGNAL(error(QString)), this, SLOT(weatherDataError(QString)));
	connect(event, SIGNAL(finished()), event, SLOT(deleteLater()));
	connect(event, SIGNAL(finished()), this, SLOT(weatherEventsDone()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(started()), event, SLOT(processCurrentWeather()));
	connect(event, SIGNAL(temperature(double)), this, SLOT(currentTemperature(double)));
	connect(event, SIGNAL(humidity(double)), this, SLOT(currentHumidity(double)));
	connect(event, SIGNAL(windSpeed(double)), this, SLOT(currentWindSpeed(double)));
	connect(event, SIGNAL(skyConditions(QString)), this, SLOT(currentSkyConditions(QString)));
	connect(event, SIGNAL(sunrise(QDateTime)), this, SLOT(sunrise(QDateTime)));
	connect(event, SIGNAL(sunset(QDateTime)), this, SLOT(sunset(QDateTime)));
	thread->start();
}

void MirrorFrame::getForecast()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MagicMirror", "MagicMirror");

	QThread* thread = new QThread;
	WeatherData *event = new WeatherData();
	event->addZip("60005");
	event->addAppID(settings.value("appid").toString());
	event->moveToThread(thread);
	connect(event, SIGNAL(finished()), thread, SLOT(quit()));
	connect(event, SIGNAL(error(QString)), this, SLOT(weatherDataError(QString)));
	connect(event, SIGNAL(finished()), event, SLOT(deleteLater()));
	connect(event, SIGNAL(finished()), this, SLOT(weatherEventsDone()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(started()), event, SLOT(processForecast()));
	connect(event, SIGNAL(forecastEntry(QJsonObject)), this, SLOT(forecastEntry(QJsonObject)));
	thread->start();
}

void MirrorFrame::sunrise(QDateTime t)
{
	m_sunrise->setText(QString("<center>%1</center>").arg(t.toString("hh:mm ap")));
}

void MirrorFrame::sunset(QDateTime t)
{
	m_sunset->setText(QString("<center>%1</center>").arg(t.toString("hh:mm ap")));
}

void MirrorFrame::weatherEventsDone()
{
	qDebug() << __PRETTY_FUNCTION__;
}

void MirrorFrame::currentTemperature(double temp)
{
	temp = temp + 0.5;
	int rounded = (int)temp;

	m_currentTemp->setText(QString("<center>%1%2</center>").arg(rounded).arg(QChar(0260)));
}

void MirrorFrame::currentSkyConditions(QString sky)
{
	m_currentSky->setText(QString("<center>%1</center>").arg(sky));
}

void MirrorFrame::currentHumidity(double humidity)
{
	humidity = humidity + 0.5;
	int rounded = (int)humidity;

	m_currentHumidity->setText(QString("<center>%1%</center>").arg(rounded));
}

void MirrorFrame::currentWindSpeed(double speed)
{
	speed = speed + 0.5;
	int rounded = (int)speed;

	m_currentWind->setText(QString("<center>%1 mph</center>").arg(rounded));
}

void MirrorFrame::weatherDataError(QString)
{
	qDebug() << __PRETTY_FUNCTION__;
}

void MirrorFrame::forecastFinished()
{
	m_forecastIndex = 0;
}

void MirrorFrame::forecastEntry(QJsonObject jobj)
{
	QDateTime dt;
	int humidity;
	double high;
	double low;
	QString sky;


	dt.setMSecsSinceEpoch(jobj["dt"].toInt() * 1000);
	humidity = jobj["humidity"].toInt();
	QJsonObject temp = jobj["temp"].toObject();
	high = temp["max"].toDouble() + 0.5;
	low = temp["min"].toDouble() + 0.5;
	QJsonArray weather = jobj["weather"].toArray();
	for (int i = 0; i < weather.size(); ++i) {
		QJsonObject obj = weather[i].toObject();
		sky = obj["main"].toString();
	}

	QLabel *lb = m_forecastEntries[m_forecastIndex++];
	lb->setText(QString("%1: It will be %2 with a high of %3, a low of %4, and humidity of %5").arg(dt.toString("MMM d")).arg(sky).arg((int)high).arg((int)low).arg(humidity));
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
