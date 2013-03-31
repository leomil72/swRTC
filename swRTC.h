/*
	swRTC.h - A software implementation of a Real-Time Clock (RTC) using the timer2 of Arduino
	Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>
	with contribution of lesto <www.arduino.cc>
	Inspired by MsTimer2 by Javier Valencia

	Current version: 1.0.6
	(For the complete version history, please refer to the README file)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public
	License as published by the Free Software Foundation; either
	version 3.0 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef swRTC_H
#define swRTC_H

//library version
#define swRTC_VERSION 110

//Library is compatible both with Arduino <=0023 and Arduino >=100
#if defined(ARDUINO) && (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//check if the micro is supported
#if defined (__AVR_ATmega48__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega48P__) || defined (__AVR_ATmega88P__) || defined (__AVR_ATmega168P__) || defined (__AVR_ATmega328P__)
#define ATMEGAx8
#elif defined (__AVR_ATtiny25__) || defined (__AVR_ATtiny45__) || defined (__AVR_ATtiny85__)
#define ATTINYx5
#elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__)
#define ATMEGA8
#elif defined (__AVR_ATtiny24__) || defined (__AVR_ATtiny44__) || defined (__AVR_ATtiny84__)
#define ATTINYx4
#elif defined (__AVR_ATmega640__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega1281__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__)
#define ATMEGAx0 
#elif defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__) || defined (__AVR_ATmega1284P__)
#define ATMEGA644
#elif defined (__AVR_ATtiny2313__) || defined (__AVR_ATtiny4313__)
#define ATTINYx313
#else
#error Sorry, microcontroller not supported!
#endif

//check if the frequency is supported
#if ((F_CPU!=1000000UL) && (F_CPU!=4000000UL) && (F_CPU!=8000000UL) && (F_CPU!=16000000UL))
#error Sorry, clock frequency not supported!
#endif

class swRTC {
	public: 
		//public methods
		swRTC();
		boolean setTime(byte hourT, byte minuteT, byte secondT); 
		boolean setDate(byte dayT, byte monthT, int yearT);
		void startRTC();
		void stopRTC();
		boolean isLeapYear(int yearT = 0);
		unsigned long getTimestamp(int yearT = 0);
		boolean setDeltaT(double deltaT = 0.0) __attribute__ ((deprecated));
        boolean setDeltaT(int deltaT = 0);
        int getDeltaT();
		byte getSeconds();
		byte getMinutes();
		byte getHours();
		byte getDay();
		byte getMonth();
		int getYear();
		boolean setClockWithTimestamp(unsigned long timeT, int yearT = 0);
		byte weekDay(unsigned long timestamp);
		byte getWeekDay();
		
	private:
		//private methods
		void setTimer();
		//private variables
		boolean isRunning;
		int yearSize(int yearT);
};

#endif
