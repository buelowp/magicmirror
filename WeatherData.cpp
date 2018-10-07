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

void WeatherData::addZip(QString z, QString c)
{
	m_zip = z;
	m_zip += ",";
	m_zip += c;
	qDebug() << __PRETTY_FUNCTION__ << ":" << m_zip;
}

void WeatherData::addAppID(QString a)
{
	qDebug() << __PRETTY_FUNCTION__ << ":" << a;
	m_appID = a;
}

void WeatherData::processCurrentWeather()
{
	QUrl u("http://api.openweathermap.org/data/2.5/weather");
	QUrlQuery query;

	qDebug() << __PRETTY_FUNCTION__;
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

	qDebug() << __PRETTY_FUNCTION__;
	query.addQueryItem("appid", m_appID);
	query.addQueryItem("zip", m_zip);
	query.addQueryItem("units", "imperial");
	query.addQueryItem("cnt", "5");
	u.setQuery(query);

	m_forecast->get(QNetworkRequest(u));
}


void WeatherData::currentReplyFinished(QNetworkReply *reply)
{
	qDebug() << __PRETTY_FUNCTION__;
	if (reply->error()) {
		qWarning() << __PRETTY_FUNCTION__ << ":" << reply->errorString();
	}
	else {
		QJsonDocument jdoc = QJsonDocument::fromJson(reply->readAll());
		QJsonObject jobj = jdoc.object();
		QJsonObject main = jobj["main"].toObject();
		emit temperature(main["temp"].toDouble());
		emit humidity(main["humidity"].toDouble());
		QJsonObject wind = jobj["wind"].toObject();
		emit windSpeed(wind["speed"].toDouble());

		QJsonObject sys = jobj["sys"].toObject();
		emit sunrise(static_cast<qint64>(sys["sunrise"].toInt()));
		emit sunset(static_cast<qint64>(sys["sunset"].toInt()));
		QJsonArray weather = jobj["weather"].toArray();
		for (int i = 0; i < weather.size(); ++i) {
			QJsonObject obj = weather[i].toObject();
			emit skyConditions(obj["main"].toString());
		}
	}
	emit finished();
}

void WeatherData::forecastReplyFinished(QNetworkReply *reply)
{
	qDebug() << __PRETTY_FUNCTION__;
	if (reply->error()) {
		qWarning() << __PRETTY_FUNCTION__ << ":" << reply->errorString();
	}
	else {
		QJsonDocument jdoc = QJsonDocument::fromJson(reply->readAll());
		QJsonObject jobj = jdoc.object();
		QJsonArray entries = jobj["list"].toArray();
        emit forecastEntryCount(entries.count());
        qDebug() << __PRETTY_FUNCTION__ << ": sending" << entries.count() << "entries to the mirror";
		for (int i = 0; i < entries.size(); i++) {
			emit forecastEntry(entries[i].toObject());
		}
	}
	emit finished();
}
