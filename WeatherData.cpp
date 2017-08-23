#include "WeatherData.h"

WeatherData::WeatherData(QObject *parent) : QObject(parent)
{
	m_forecast = new QNetworkAccessManager(this);
	m_current = new QNetworkAccessManager(this);

	connect(m_forecast, SIGNAL(finished(QNetworkReply*)), this, SLOT(forecastReplyFinished(QNetworkReply*)));
	connect(m_current, SIGNAL(finished(QNetworkReply*)), this, SLOT(currentReplyFinished(QNetworkReply*)));
}

WeatherData::~WeatherData()
{
}


void WeatherData::addZip(QString z)
{
	m_zip = z;
}

void WeatherData::addAppID(QString a)
{
	m_appID = a;
	qDebug() << __PRETTY_FUNCTION__ << ":" << m_appID;
}

void WeatherData::processCurrentWeather()
{
	QUrl u("http://api.openweathermap.org/data/2.5/weather");
	QUrlQuery query;

	query.addQueryItem("appid", m_appID);
	query.addQueryItem("zip", m_zip);
	query.addQueryItem("units", "imperial");
	u.setQuery(query);

	m_current->get(QNetworkRequest(u));
}

void WeatherData::processForecast()
{
	QUrl u("http://api.openweathermap.org/data/2.5/forecast/daily");
	QUrlQuery query;

	query.addQueryItem("appid", m_appID);
	query.addQueryItem("zip", m_zip);
	query.addQueryItem("units", "imperial");
	query.addQueryItem("cnt", "3");
	u.setQuery(query);

	m_forecast->get(QNetworkRequest(u));
}


void WeatherData::currentReplyFinished(QNetworkReply *reply)
{
	if (reply->error()) {
		qWarning() << __PRETTY_FUNCTION__ << ":" << reply->errorString();
	}
	else {
		qDebug() << __PRETTY_FUNCTION__;
		QJsonDocument jdoc = QJsonDocument::fromJson(reply->readAll());
		QJsonObject jobj = jdoc.object();
		QJsonObject main = jobj["main"].toObject();
		emit temperature(main["temp"].toDouble());
		emit humidity(main["humidity"].toDouble());
		QJsonObject wind = jobj["wind"].toObject();
		emit windSpeed(wind["speed"].toDouble());

		QJsonObject sys = jobj["sys"].toObject();
		QDateTime t;
		t.setMSecsSinceEpoch(sys["sunrise"].toInt() * 1000);
		if (t.isValid())
			emit sunrise(t);
		else
			qDebug() << __PRETTY_FUNCTION__ << ": t isn't valid, seconds are" << sys["sunrise"].toInt();

		QDateTime t2;
		t2.setMSecsSinceEpoch(sys["sunset"].toInt() * 1000);
		if (t2.isValid())
			emit sunset(t2);
		else
			qDebug() << __PRETTY_FUNCTION__ << ": t2 isn't valid, seconds are" << sys["sunset"].toInt();

		QJsonArray weather = jobj["weather"].toArray();
		for (int i = 0; i < weather.size(); ++i) {
			QJsonObject obj = weather[i].toObject();
			emit skyConditions(obj["main"].toString());
		}
	}
}

void WeatherData::forecastReplyFinished(QNetworkReply *reply)
{
	if (reply->error()) {
		qWarning() << __PRETTY_FUNCTION__ << ":" << reply->errorString();
	}
	else {
		qDebug() << __PRETTY_FUNCTION__;
		QJsonDocument jdoc = QJsonDocument::fromJson(reply->readAll());
		QJsonObject jobj = jdoc.object();
		QJsonArray entries = jobj["list"].toArray();
		for (int i = 0; i < entries.size(); i++) {
			emit forecastEntry(entries[i].toObject());
		}
	}
	emit forecastFinished();
}
