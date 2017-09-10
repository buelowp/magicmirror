/*
 *
 *  Created on: Sep 4, 2017
 *      Author: pete
 */

#ifndef DS18B20_DS18B20_H_
#define DS18B20_DS18B20_H_

#include <iostream>
#include <fstream>
#include <string>
#include <QtCore/QtCore>

class DS18B20 {
public:
	DS18B20();
	virtual ~DS18B20();

	bool open(const char*);
	void read();
	float tempC();
	float tempF();
	void close();

private:
	std::ifstream m_device;
	float m_value;
};

#endif /* DS18B20_DS18B20_H_ */
