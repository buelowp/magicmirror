#include "MirrorFrame.h"

MirrorFrame::MirrorFrame(QFrame *parent) : QFrame(parent)
{

	m_calendarTimer = new QTimer();
	m_forecastTimer = new QTimer();
	m_currentWeatherTimer = new QTimer();
	m_clockTimer = new QTimer();
	m_localTempTimer = new QTimer();
	m_monitorTimer = new QTimer();
	m_monitorState = new QStateMachine(this);
	
	QPalette pal(QColor(0,0,0));
	setBackgroundRole(QPalette::Window);
	pal.setColor(QPalette::Window, Qt::black);
	setAutoFillBackground(true);
	setPalette(pal);

	m_calLabel = new QLabel("<font color='white'>Upcoming Events", this);
	m_forecastLabel = new QLabel("<font color='white'>Local Forecast</font>", this);
	m_currentLabel = new QLabel("<font color='white'>Current Conditions</font>", this);
	m_clockLabel = new QLabel(this);

	QFont f("Roboto");
	f.setPixelSize(50);
	f.setBold(true);
	m_calLabel->setGeometry(50, 10, 500, 100);
	m_calLabel->setFont(f);
	m_currentLabel->setGeometry(50, 900, 600, 100);
	m_currentLabel->setFont(f);
	m_forecastLabel->setGeometry(50, 1400, 500, 100);
	m_forecastLabel->setFont(f);

	m_clockLabel->setGeometry(700, 10, 650, 100);
	f.setPixelSize(40);
	m_clockLabel->setFont(f);

	f.setPixelSize(30);
	m_currentTempLabel = new QLabel(this);
	m_currentHumidityLabel = new QLabel(this);
	m_currentWindLabel = new QLabel(this);
	m_currentSkyLabel = new QLabel(this);
	m_localTempLabel = new QLabel(this);
	m_localHumidityLabel = new QLabel(this);

	m_localTempLabel->setGeometry(50, 1000, 350, 50);
	m_localHumidityLabel->setGeometry(400, 1000, 350, 50);
	m_localTempLabel->setFont(f);
	m_localHumidityLabel->setFont(f);
	m_localTempLabel->setText("<center>Inside Temperature</center>");
	m_localHumidityLabel->setText("<center>Inside Humidity</center>");

	m_currentTempLabel = new QLabel(this);
	m_currentTempLabel->setGeometry(50, 1100, 350, 50);
	m_currentTempLabel->setFont(f);
	m_currentTempLabel->setText("<center>Outside Temperature</center>");

	m_currentHumidityLabel = new QLabel(this);
	m_currentHumidityLabel->setGeometry(400, 1100, 350, 50);
	m_currentHumidityLabel->setFont(f);
	m_currentHumidityLabel->setText("<center>Outside Humidity</center>");

	m_sunriseLabel = new QLabel(this);
	m_sunriseLabel->setGeometry(750, 1100, 350, 50);
	m_sunriseLabel->setFont(f);
	m_sunriseLabel->setText("<center>Sunrise</center>");

	m_currentWindLabel = new QLabel(this);
	m_currentWindLabel->setGeometry(50, 1200, 350, 50);
	m_currentWindLabel->setFont(f);
	m_currentWindLabel->setText("<center>Wind Speed</center>");

	m_currentSkyLabel = new QLabel(this);
	m_currentSkyLabel->setGeometry(400, 1200, 350, 50);
	m_currentSkyLabel->setFont(f);
	m_currentSkyLabel->setText("<center>Sky Conditions</center>");

	m_sunsetLabel = new QLabel(this);
	m_sunsetLabel->setGeometry(750, 1200, 350, 50);
	m_sunsetLabel->setFont(f);
	m_sunsetLabel->setText("<center>Sunset</center>");

	f.setPixelSize(25);
	f.setBold(false);

	m_localTemp = new QLabel(this);
	m_localTemp->setGeometry(50, 1050, 350, 50);
	m_localTemp->setFont(f);

	m_localHumidity = new QLabel(this);
	m_localHumidity->setGeometry(400, 1050, 350, 50);
	m_localHumidity->setFont(f);

	m_currentTemp = new QLabel(this);
	m_currentTemp->setGeometry(50, 1150, 350, 50);
	m_currentTemp->setFont(f);

	m_currentHumidity = new QLabel(this);
	m_currentHumidity->setGeometry(400, 1150, 350, 50);
	m_currentHumidity->setFont(f);

	m_sunrise = new QLabel(this);
	m_sunrise->setGeometry(750, 1150, 350, 50);
	m_sunrise->setFont(f);

	m_currentWind = new QLabel(this);
	m_currentWind->setGeometry(50, 1250, 350, 50);
	m_currentWind->setFont(f);

	m_currentSky = new QLabel(this);
	m_currentSky->setGeometry(400, 1250, 350, 50);
	m_currentSky->setFont(f);

	m_sunset = new QLabel(this);
	m_sunset->setGeometry(750, 1250, 350, 50);
	m_sunset->setFont(f);

	for (int i = 0; i < 5; i++) {
		QLabel *lb = new QLabel(this);
		lb->setGeometry(50, ((i * 50) + 1500), 1200, 50);
		lb->setFont(f);
		m_forecastEntries.push_back(lb);
	}

	m_calEventsY = 110;
	m_forecastIndex = 0;
	m_newEventList = false;
	m_resetForecastTimer = true;
    m_forecastEntryCount = 0;
    m_temperature = 0.0;
    m_humidity = 0.0;
	
	createStateMachine();
	enableTimers();

	updateLocalTemp();
}

MirrorFrame::~MirrorFrame()
{
}

void MirrorFrame::registerTouchEvent()
{
	emit touchDetected();
}

void MirrorFrame::createStateMachine()
{
	QState *on = new QState();
	QState *off = new QState();
	
	off->addTransition(this, SIGNAL(touchDetected()), on);
	on->addTransition(m_monitorTimer, SIGNAL(timeout()), off);
	connect(on, SIGNAL(entered()), this, SLOT(monitorOn()));
	connect(off, SIGNAL(entered()), this, SLOT(monitorOff()));
	connect(this, SIGNAL(touchDetected()), this, SLOT(resetMonitorTimer()));
	m_monitorState->addState(on);
	m_monitorState->addState(off);
	m_monitorState->setInitialState(on);
	m_monitorState->start();
}

void MirrorFrame::enableTimers()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MagicMirror", "MagicMirror");
	int monitorTimeout = settings.value("screentimeout").toInt();
	QDateTime now = QDateTime::currentDateTime();
	QDateTime midnight;
	QDate tomorrow(now.date().year(), now.date().month(), now.date().day());
	tomorrow = tomorrow.addDays(1);
	midnight.setDate(tomorrow);
		
	connect(m_calendarTimer, SIGNAL(timeout()), this, SLOT(getEvents()));
	m_calendarTimer->start(CALEVENTS_TIMEOUT);		// Get Events once an hour
	
	connect(m_forecastTimer, SIGNAL(timeout()), this, SLOT(getForecast()));
	m_forecastTimer->start(now.msecsTo(midnight));		// set to timeout at midnight, we reset it to 12 hours later
	
	connect(m_currentWeatherTimer, SIGNAL(timeout()), this, SLOT(getCurrentWeather()));
	m_currentWeatherTimer->start(CURRENT_TIMEOUT);	// get current weather conditions once an hour
	
	connect(m_clockTimer, SIGNAL(timeout()), this, SLOT(updateClock()));
	m_clockTimer->start(500);					// Update the clock 1x a second

	connect(m_localTempTimer, SIGNAL(timeout()), this, SLOT(updateLocalTemp()));
	m_localTempTimer->start(1000 * 60);

	if (monitorTimeout == 0)
		monitorTimeout = MONITOR_TIMEOUT;
	else
		monitorTimeout = monitorTimeout * 1000 * 60;

	qDebug() << __PRETTY_FUNCTION__ << ": setting monitor timeout to" << monitorTimeout;
	m_monitorTimer->start(monitorTimeout);
}

void MirrorFrame::updateLocalTemp()
{
#ifdef __USE_RPI__
	if (getValues(&t, &h) == 0) {
		m_temperature = t;
		m_humidity = h;
	}
	qDebug() << __PRETTY_FUNCTION__ << ": temp: " << m_temperature << ", humidity: " << m_humidity;
#endif
	m_localTemp->setText(QString("<center>%1%2</center>").arg(m_temperature, 0, 'f', 1).arg(QChar(0260)));
	m_localHumidity->setText(QString("<center>%1%</center>").arg(m_humidity, 0, 'f', 1));
}

void MirrorFrame::resetMonitorTimer()
{
	qDebug() << __PRETTY_FUNCTION__;
	if (m_monitorTimer->isActive())
		m_monitorTimer->stop();

	m_monitorTimer->start(MONITOR_TIMEOUT);
}

void MirrorFrame::monitorOff()
{
	qDebug() << __PRETTY_FUNCTION__;
	turnMonitorOff();
	if (m_monitorTimer->isActive())
		m_monitorTimer->stop();
}

void MirrorFrame::monitorOn()
{
	qDebug() << __PRETTY_FUNCTION__;
	if (!m_monitorTimer->isActive()) {
		m_monitorTimer->setInterval(MONITOR_TIMEOUT);
		m_monitorTimer->start();
	}
	turnMonitorOn();
}

void MirrorFrame::turnMonitorOff()
{
	QProcess p;
	
	qDebug() << __PRETTY_FUNCTION__;
	p.start("vcgencmd", QStringList() << "display_power" << "0");
	p.waitForFinished();
}

void MirrorFrame::turnMonitorOn()
{
	QProcess p;
	
	qDebug() << __PRETTY_FUNCTION__;
	p.start("vcgencmd", QStringList() << "display_power" << "1");
	p.waitForFinished();
}

void MirrorFrame::updateClock()
{
	QDateTime now = QDateTime::currentDateTime();
	m_clockLabel->setText(now.toString("ddd MMM d h:mm ap"));
}

void MirrorFrame::getCurrentWeather()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MagicMirror", "MagicMirror");

	qDebug() << __PRETTY_FUNCTION__;
	QThread* thread = new QThread;
	WeatherData *event = new WeatherData();
	event->addZip(settings.value("zip").toString(), settings.value("country").toString());
	event->addAppID(settings.value("appid").toString());
	event->moveToThread(thread);
    event->setThreadPointer(thread);
	connect(event, SIGNAL(finished()), thread, SLOT(quit()));
	connect(event, SIGNAL(error(QString)), this, SLOT(weatherDataError(QString)));
	connect(event, SIGNAL(finished()), thread, SLOT(quit()));
	connect(event, SIGNAL(finished()), this, SLOT(weatherEventsDone()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(started()), event, SLOT(processCurrentWeather()));
	connect(event, SIGNAL(temperature(double)), this, SLOT(currentTemperature(double)));
	connect(event, SIGNAL(humidity(double)), this, SLOT(currentHumidity(double)));
	connect(event, SIGNAL(windSpeed(double)), this, SLOT(currentWindSpeed(double)));
	connect(event, SIGNAL(skyConditions(QString)), this, SLOT(currentSkyConditions(QString)));
	connect(event, SIGNAL(sunrise(qint64)), this, SLOT(sunrise(qint64)));
	connect(event, SIGNAL(sunset(qint64)), this, SLOT(sunset(qint64)));
    connect(event, SIGNAL(forecastEntryCount(int)), this, SLOT(forecastEntryCount(int)));
	thread->start();
}

void MirrorFrame::getForecast()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "MagicMirror", "MagicMirror");

	qDebug() << __PRETTY_FUNCTION__;
	QThread* thread = new QThread;
	WeatherData *event = new WeatherData();
	event->addZip(settings.value("zip").toString(), settings.value("country").toString());
	event->addAppID(settings.value("appid").toString());
	event->moveToThread(thread);
    event->setThreadPointer(thread);
	connect(event, SIGNAL(finished()), thread, SLOT(quit()));
	connect(event, SIGNAL(error(QString)), this, SLOT(weatherDataError(QString)));
	connect(event, SIGNAL(finished()), event, SLOT(deleteLater()));
	connect(event, SIGNAL(finished()), this, SLOT(weatherEventsDone()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	connect(thread, SIGNAL(started()), event, SLOT(processForecast()));
	connect(event, SIGNAL(forecastEntry(QJsonObject)), this, SLOT(forecastEntry(QJsonObject)));
    connect(event, SIGNAL(forecastEntryCount(int)), this, SLOT(forecastEntryCount(int)));
	thread->start();
	m_resetForecastTimer = false;
	m_forecastTimer->setInterval(FORECAST_TIMEOUT);		// Run the forecaster every 12 hours
}

void MirrorFrame::sunrise(qint64 t)
{
	qDebug() << __PRETTY_FUNCTION__;
	QDateTime s;
	s.setMSecsSinceEpoch(t * 1000);
	m_sunrise->setText(QString("<center>%1</center>").arg(s.toString("hh:mm ap")));
}

void MirrorFrame::sunset(qint64 t)
{
	qDebug() << __PRETTY_FUNCTION__;
	QDateTime s;
	s.setMSecsSinceEpoch(t * 1000);
	m_sunset->setText(QString("<center>%1</center>").arg(s.toString("hh:mm ap")));
}

void MirrorFrame::weatherEventsDone()
{
	qDebug() << __PRETTY_FUNCTION__;
}

void MirrorFrame::currentTemperature(double temp)
{
	qDebug() << __PRETTY_FUNCTION__;
	m_currentTemp->setText(QString("<center>%1%2</center>").arg(temp, 0, 'f', 1).arg(QChar(0260)));
}

void MirrorFrame::currentSkyConditions(QString sky)
{
	qDebug() << __PRETTY_FUNCTION__;
	m_currentSky->setText(QString("<center>%1</center>").arg(sky));
}

void MirrorFrame::currentHumidity(double humidity)
{
	qDebug() << __PRETTY_FUNCTION__;
	m_currentHumidity->setText(QString("<center>%1%</center>").arg(humidity));
}

void MirrorFrame::currentWindSpeed(double speed)
{
	speed = speed + 0.5;
	int rounded = (int)speed;

	qDebug() << __PRETTY_FUNCTION__;
	m_currentWind->setText(QString("<center>%1 mph</center>").arg(rounded));
}

void MirrorFrame::weatherDataError(QString)
{
	qDebug() << __PRETTY_FUNCTION__;
}

void MirrorFrame::forecastEntryCount(int c)
{
    m_forecastEntryCount = c;
    m_forecastIndex = 0;
    qDebug() << __PRETTY_FUNCTION__ << ": There are" << m_forecastEntryCount << "responses in the forecast";
}

void MirrorFrame::forecastEntry(QJsonObject jobj)
{
    QDateTime dt;
    QDateTime now = QDateTime::currentDateTime();
    int humidity;
    double high;
    double low;
    double wind;
    QString sky;

    qDebug() << __PRETTY_FUNCTION__ << jobj;
    qint64 secs = jobj["dt"].toInt();
    secs *= 1000;
    dt.setMSecsSinceEpoch(secs);
    humidity = jobj["humidity"].toInt();
    QJsonObject temp = jobj["temp"].toObject();
    high = temp["max"].toDouble() + 0.5;
    low = temp["min"].toDouble() + 0.5;
    wind = jobj["speed"].toDouble();
    
    QJsonArray weather = jobj["weather"].toArray();
    for (int i = 0; i < weather.size(); ++i) {
        QJsonObject obj = weather[i].toObject();
        sky = obj["main"].toString();
    }

    if (m_forecastIndex < m_forecastEntries.size()) {
        qDebug() << __PRETTY_FUNCTION__ << ": Updating entry index " << m_forecastIndex;
        QLabel *lb = m_forecastEntries[m_forecastIndex++];
        if (now.date() == dt.date()) {
            QString text = QString("Today's high: %1%2, low: %3%4, %5")
                .arg((int)high)
                .arg(QChar(0260))
                .arg((int)low)
                .arg(QChar(0260))
                .arg(sky);

            if (wind <= 5.0) {
                text.append(", calm");
            }
            else if (wind <= 10.0) {
                text.append(", breezy");
            }
            else if (wind < 15.0) {
                text.append(", windy");
            }
            else {
                text.append(", very windy");
            }
            if (humidity > 75) {
                text.append(" and very humid");
            }
            else if (humidity < 55) {
                text.append(" feeling dry");
            }
            lb->setText(text);
        }
        else {
            QString text = QString("%1: high: %2%3, low: %4%5: %6")
                .arg(dt.toString("MMM d"))
                .arg((int)high)
                .arg(QChar(0260))
                .arg((int)low)
                .arg(QChar(0260))
                .arg(sky);

            if (wind <= 5.0) {
                text.append(", calm");
            }
            else if (wind <= 10.0) {
                text.append(", breezy");
            }
            else if (wind < 15.0) {
                text.append(", windy");
            }
            else {
                text.append(", very windy");
            }
            if (humidity > 75) {
                text.append(" and very humid");
            }
            else if (humidity < 55) {
                text.append(" and dry");
            }
            lb->setText(text);
        }
    }
}

void MirrorFrame::getEvents()
{
	qDebug() << __PRETTY_FUNCTION__;
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
	qDebug() << __PRETTY_FUNCTION__;
	m_newEventList = true;
}

void MirrorFrame::calendarEventsEvent(QString s)
{
	if (m_newEventList)
		deleteCalendarEventsList();

	qDebug() << __PRETTY_FUNCTION__;
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
