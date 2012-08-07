/*
	swRTC.h - A software implementation of a Real-Time Clock (RTC) using the timer2 of Arduino
	Written by Leonardo Miliani <leonardo AT leonardomiliani DOT com>
	with contribution of lesto <www.arduino.cc>
	Inspired by MsTimer2 by Javier Valencia

	History:
	v. 1.0.5: introduced new method getWeekDay
	v. 1.0.4: added support for Attiny4313
	v. 1.0.3: added support for 4 MHz clock
	v. 1.0.2: first fix of a bug in the algorithm of getTimestamp
	v. 1.0.1: latest versione, distribuited under the terms of the GNU GPL v.3.0 and any later version
	v. 1.0:   first stable & complete release of the library
	v. 0.8.4: the function now can set the internal clock using a timestamp
	v. 0.8.3: introduced support for Atmega1284
	v. 0.8.2: fixed a glitch in deltaT and introduced support for deltaT with decimals
	v. 0.8.1: new deltaT based on seconds/day
	v. 0.8:   introduced deltaT to correct deviations between computed & real time & Timestamp
	v. 0.7.1: dropped millis sketch due to the fact that milliseconds is no longer required
	v. 0.7:   corrected some bug; the support for Attinyx4 now works
	v. 0.6:   added supporto for Attiny2313
	v. 0.5:   corrected some bugs 
	v. 0.4:   addedd support for Atmega640/1280/1281/2560/2561
	v. 0.3:   added support for Attiny24/44/84 and Atmega644/Atmega644P (Attinyx4 don't work at the moment: FIXED)
	v. 0.2:   added support for Attiny25/45/85 and Atmega8/8A
	v. 0.1:   first release - only works on Arduino and Atmega48/88/168/328/328P at 1/8/16 MHz

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
		boolean isLeapYear(int yearT=NULL);
		unsigned long getTimestamp(int yearT=NULL);
		boolean setDeltaT(float deltaT=0.0);
		byte getSeconds();
		byte getMinutes();
		byte getHours();
		byte getDay();
		byte getMonth();
		int getYear();
		boolean setClockWithTimestamp(unsigned long timeT, int yearT=NULL);
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
