/*
 *
 *  Created on: Sep 4, 2017
 *      Author: pete
 */

#include "ds18b20.h"

DS18B20::DS18B20()
{
}

DS18B20::~DS18B20()
{
	m_device.close();
}

bool DS18B20::open(const char *file)
{
	m_device.open(file);
	if (!m_device.is_open())
		return false;

	qDebug() << __PRETTY_FUNCTION__ << ":" << file;
	return true;
}

void DS18B20::close()
{
	m_device.close();
}

void DS18B20::read()
{
	char buff[128];
	std::string data;
	bool bFoundCrcOk=false;

	static const std::string tag("t=");

	if (m_device.is_open()) {
		std::string sLine;
		while (!m_device.eof()) {
			m_device.getline(buff, 128);
			sLine = buff;
			int tpos;
			if (sLine.find("crc=")!=std::string::npos) {
				if (sLine.find("YES")!=std::string::npos)
					bFoundCrcOk=true;
			}
			else if ((tpos=sLine.find(tag))!=std::string::npos) {
				data = sLine.substr(tpos+tag.length());
			}
		}
	}

	if (bFoundCrcOk) {
		m_value = std::atof(data.c_str());
	}
}

float DS18B20::tempC()
{
	return (m_value / 1000);
}

float DS18B20::tempF()
{
	return ((m_value / 1000) * 9 / 5 + 32);
}

