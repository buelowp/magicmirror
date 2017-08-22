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
}

void WeatherData::process()
{
	processCurrentWeather();
	processForecast();
}

void WeatherData::processCurrentWeather()
{
	QUrl u("http://api.openweathermap.org/data/2.5/weather");
	QUrlQuery query;

	query.addQueryItem("appid", m_appID);
	query.addQueryItem("zip", m_zip);
	query.addQueryItem("units", "imperial");
	u.setQuery(query);

	qDebug() << __PRETTY_FUNCTION__ << ":" << u;
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

	qDebug() << __PRETTY_FUNCTION__ << ":" << u;
	m_forecast->get(QNetworkRequest(u));
}


void WeatherData::currentReplyFinished(QNetworkReply *reply)
{
	if (reply->error()) {
		qWarning() << __PRETTY_FUNCTION__ << ":" << reply->errorString();
	}
	else {
		QJsonDocument jdoc = QJsonDocument::fromJson(reply->readAll());
		QJsonObject jobj = jdoc.object();
		QMap<QString,QString> forecast;
		forecast["temp"] = jobj["temp"].toString();
		forecast["humidity"] = jobj["humidity"].toString();
		forecast["wind"] = jobj["speed"].toString();
		QJsonArray weather = jobj["weather"].toArray();
		for (int i = 0; i < weather.size(); ++i) {
			QJsonObject obj = weather[i].toObject();
			forecast["sky"] = obj["description"].toString();
		}
		emit currentConditions(forecast);
	}
}

void WeatherData::forecastReplyFinished(QNetworkReply *reply)
{
	if (reply->error()) {
		qWarning() << __PRETTY_FUNCTION__ << ":" << reply->errorString();
	}
	else {
		QJsonDocument jdoc = QJsonDocument::fromJson(reply->readAll());
		if (!jdoc.isNull()) {
			qDebug() << jdoc.toJson(QJsonDocument::Indented);
		}
		else
			qWarning() << __PRETTY_FUNCTION__ << ": Unable to parse reply";
	}
}
