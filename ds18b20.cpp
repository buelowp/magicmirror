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
	std::string results;
	char *buff;
	int length = 0;
	unsigned int index = 0;

    m_device.seekg(0, m_device.end);
    length = m_device.tellg();
    m_device.seekg(0, m_device.beg);

    buff = new char[length];
    m_device.read(buff, length);
	qDebug() << __PRETTY_FUNCTION__ << ": file size is" << length;
    results = buff;

    if ((index = results.find("t=")) != std::string::npos) {
    	std::string val = results.substr(index + 2, 5);
    	m_value = std::stof(val);
	qDebug() << __PRETTY_FUNCTION__ << ": m_value = " << m_value;
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

