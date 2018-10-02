#ifndef __WEATHERDATA_H__
#define __WEATHERDATA_H__

#include <QtCore/QtCore>
#include <QtNetwork/QtNetwork>

class WeatherData : public QObject
{
	Q_OBJECT
public:
	WeatherData(QObject *parent = 0);
	virtual ~WeatherData();

	void addZip(QString, QString);
	void addAppID(QString);

signals:
	void currentConditions(QMap<QString,QString>);
	void forecastEntry(QJsonObject);
	void finished();
	void error(QString);
	void temperature(double);
	void humidity(double);
	void windSpeed(double);
	void skyConditions(QString);
	void sunrise(qint64);
	void sunset(qint64);
	void forecastFinished();
	void currentConditionsFinished();
    void forecastEntryCount(int);

public slots:
	void processForecast();
	void processCurrentWeather();
	void forecastReplyFinished(QNetworkReply*);
	void currentReplyFinished(QNetworkReply*);

private:
	QNetworkAccessManager *m_forecast;
	QNetworkAccessManager *m_current;
	QString m_appID;
	QString m_zip;
};

#endif
