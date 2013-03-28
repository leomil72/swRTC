/* This file is part of swRTC library.
   Please read the README file and the notes inside the swRTC.h file
   to read about the usage of this code.
*/

//Now the library is compatible both with Arduino <=0023 and Arduino >=100
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//include required libraries
#include "swRTC.h"
#include <avr/interrupt.h>

//global variables
swRTC *lib;
volatile int delta=0;
volatile long deltaS=0;
volatile int8_t deltaDir=0;
volatile byte starter=0;
volatile int counterT=0;
volatile byte hours=0;
volatile byte minutes=0;
volatile byte secondsX=0;
volatile byte day=0;
volatile byte month=0;
volatile int year=0;
volatile byte daysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//instructions to execute when a new istance of the lib is created
swRTC::swRTC(void) {
	isRunning=false;
	lib=this;
	setTimer();
	counterT=0;
	delta=0;
}


//
//private methods
//

/*
    *****  WARNING  *****
    DO NOT MODIFY THE FOLLOWING FUNCTIONS BETWEEN HERE......
*/
//set the timer
void swRTC::setTimer() {
	float prescaler = 0.0;

#if defined (ATMEGAx8) || defined (ATMEGA644) || defined (ATMEGAx0)
	//during setup, disable all the interrupts based on timer
	TIMSK2 &= ~(1<<TOIE2); 
	TIMSK2 &= ~((1<<OCIE2A) | (1<<OCIE2B));
	//prescaler source clock set to internal Atmega clock (asynch mode)
	ASSR &= ~(1<<AS2);
	//this sets the timer to increment the counter until overflow
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20)); 
	TCCR2B &= ~(1<<WGM22);
	//the following code sets the prescaler depending on the system clock
	if (F_CPU == 16000000UL) {   // prescaler set to 64
		TCCR2B |= (1<<CS22);
		TCCR2B &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if ((F_CPU == 8000000UL) || (F_CPU == 4000000UL)) { // prescaler set to 32
		TCCR2B &= ~(1<<CS22); 
		TCCR2B |= ((1<<CS21) | (1<<CS20)); 
		prescaler = 32.0;
	} else if (F_CPU == 1000000UL) { // prescaler set to 8
		TCCR2B &= ~((1<<CS22) | (1<<CS20));
		TCCR2B |= (1<<CS21);
		prescaler = 8.0;
	}
#elif defined (ATTINYx5) || defined (ATTINYx313)
	//during setup, disable all the interrupts based on timer 0
	TIMSK &= ~(1<<TOIE0); 
	TIMSK &= ~((1<<OCIE0A) | (1<<OCIE0B));
	//normal mode: counter not connected to external pins
	TCCR0A &= ~((1<<COM0A0) | (1<<COM0A1));
	//this sets the timer to increment the counter until overflow
	TCCR0A &= ~((1<<WGM01) | (1<<WGM00));
	TCCR0B &= ~(1<<WGM02);
	//the following code sets the prescaler depending on the system clock
	if ((F_CPU == 16000000UL) || (F_CPU == 8000000UL)) {   // prescaler set to 64
		TCCR0B &= ~(1<<CS02);
		TCCR0B |= ((1<<CS01) | (1<<CS00));
		prescaler = 64.0;
	} else if (F_CPU == 1000000UL) { // prescaler set to 8
		TCCR0B &= ~((1<<CS02) | (1<<CS00));
		TCCR0B |= (1<<CS01);
		prescaler = 8.0;
	}
#elif defined (ATTINYx4)
	//on Attinyx4 we must use the timer 0 because timer1 is a 16 bit counter
	
	//during setup, disable all the interrupts based on timer 0
	TIMSK0 &= ~(1<<TOIE0);
	TIMSK0 &= ~((1<<OCIE0A) | (1<<OCIE0B));
	//normal mode: increment counter until overflow
	TCCR0B &= ~(1<<WGM02);
	TCCR0A &= ~((1<<WGM01) | (1<<WGM00));
	//normal mode: disconnect timer from pins
	TCCR0A &= ~((1<<COM0A0) | (1<<COM0A1));
	//the following code sets the prescaler depending on the system clock
	if ((F_CPU == 16000000UL) || (F_CPU == 8000000UL)) {   // prescaler set to 64
		TCCR0B &= ~(1<<CS02);
		TCCR0B |= ((1<<CS01) | (1<<CS00));
		prescaler = 64.0;
	} else if (F_CPU == 1000000UL) { // prescaler set to 8
		TCCR0B &= ~((1<<CS02) | (1<<CS00));
		TCCR0B |= (1<<CS01);
		prescaler = 8.0;
	}
#elif defined (ATMEGA8)
	//during setup, disable all the interrupts based on timer1
	TIMSK &= ~(1<<TOIE2);
	TIMSK &= ~(1<<OCIE2);
	//normal mode: counter incremented until overflow
	TCCR2 &= ~((1<<WGM21) | (1<<WGM20));
	//prescaler source clock set to internal Atmega clock (asynch mode)
	ASSR &= ~(1<<AS2);
	
	if (F_CPU == 1600000UL) {	// prescaler set to 64
		TCCR2 |= (1<<CS22);
		TCCR2 &= ~((1<<CS21) | (1<<CS20));
		prescaler = 64.0;
	} else if ((F_CPU == 8000000UL) || (F_CPU == 4000000UL)) {	// prescaler set to 32
		TCCR2 &= ~(1<<CS22);
		TCCR2 |= ((1<<CS21) | (1<<CS20));
		prescaler = 32.0;
	} else if (F_CPU == 1000000L) { // prescaler set to 8
		TCCR2 |= (1<<CS21);
		TCCR2 &= ~((1<<CS22) | (1<<CS20));
		prescaler = 8.0;
	}
#endif

	//set the initial value of the counter depending on the prescaler
	starter = 256 - (int)((float)F_CPU * 0.001 / prescaler);
}


//interrupt-driven routine to update the software clock
#if defined (ATMEGAx8) || defined (ATMEGA8) || defined (ATMEGA644) || defined (ATMEGAx0)
ISR(TIMER2_OVF_vect) {
	TCNT2 = starter;
#elif defined (ATTINYx313)
ISR(TIMER0_OVF_vect) {
	TCNT0 = starter;
#elif defined (ATTINYx4) || defined (ATTINYx5)
ISR (TIM0_OVF_vect) {
	TCNT0 = starter;
#endif
	byte dayT;
	counterT++;
	if (delta!=0) {
		deltaS--;
		if (deltaS<=0) {
			if ((deltaDir>0) && (counterT++<1000)) {
				deltaS=864000/delta;
				counterT++;
			}
			if ((deltaDir<0) && (counterT-->=0)) {
				deltaS=864000/delta;
				counterT--;
			}
		}
	}
		
	if (counterT>999) { //1000 ms equals to 1 s 
		counterT=0;
		secondsX++;
		if (secondsX>59) {
			secondsX=0;
			minutes++;
			if (minutes>59) {
				minutes=0;
				hours++;
				if (hours>23) {
					hours=0;
					day++;
					if (month == 2) { //february?
						if ((*lib).isLeapYear()) {
							dayT=29;
						} else {
							dayT=28;
						}
					} else {
						dayT=daysPerMonth[month-1];
					}
					if (day>dayT) {
						day=1;
						month++;
						if (month>12) {
							month=1;
							year++;
						}
					}
				}
			}
		}
	}
}
 
/*
    .....AND HERE, UNLESS YOU EXTACLY KNOW WHAT YOU'RE DOING!
    YOU COULD ALTER THE TIMEKEEPING ALGHORITHM
*/

/**********************************************************************/

//
//public methods
//

//sets the time
boolean swRTC::setTime(byte hourT, byte minuteT, byte secondT) {

	//check if the params are correct
	if ((hourT < 0) || (hourT > 23)) { return false; }
	if ((minuteT < 0) || (minuteT > 59)) { return false; }
	if ((secondT < 0) || (secondT > 59)) { return false; }

	hours=hourT;
	minutes=minuteT;
	secondsX=secondT;
	return true;
}


//sets the date
boolean swRTC::setDate(byte dayT, byte monthT, int yearT) {

	//check if the params are correct
	if ((dayT < 1) || (dayT > 31)) { return false; }
	if ((monthT < 1) || (monthT > 12)) { return false; }
	if (yearT < 0)  { return false; }

	day=dayT;
	month=monthT;
	year=yearT;
	return true;
}


//start the software RTC
void swRTC::startRTC() {
	setTimer();
	counterT = 0;
#if defined (ATMEGAx8) || defined (ATMEGA644) || defined (ATMEGAx0)
	TCNT2 = starter;
	TIMSK2 |= (1<<TOIE2);
#elif defined (ATMEGA8)
	TCNT2 = starter;
	TIMSK |= (1<<TOIE2);
#elif defined (ATTINYx5) || defined (ATTINYx313)
	TCNT0 = starter;
	TIMSK |= (1<<TOIE0);
#elif defined (ATTINYx4)
	TCNT0 = starter;
	TIMSK0 |= (1<<TOIE0);
#endif
	SREG |= (1<<SREG_I);
	isRunning=true;
}


void swRTC::stopRTC() {
#if defined (ATMEGAx8) || defined (ATMEGA644) || defined (ATMEGAx0)
	TIMSK2 &= ~(1<<TOIE2);
#elif defined (ATMEGA8)
	TIMSK &= ~(1<<TOIE2);
#elif defined (ATTINYx5) || defined (ATTINYx313)
	TIMSK &= ~(1<<TOIE0);
#elif defined (ATTINYx4)
	TIMSK0 &= ~(1<<TOIE0);
#endif
	SREG &= ~(1<<SREG_I);
	isRunning=false;
}


//returns the seconds
byte swRTC::getSeconds() {
	return secondsX;
}


//return the minutes
byte swRTC::getMinutes() {
	return minutes;
}


//return the hours
byte swRTC::getHours() {
	return hours;
}


//return the day
byte swRTC::getDay() {
	return day;
}


//return the month
byte swRTC::getMonth() {
	return month;
}


//return the year
int swRTC::getYear() {
	return year;
}


//return the correction
int swRTC::getDeltaT() {
	return delta;
}


//check if the current year is a leap year
boolean swRTC::isLeapYear(int yearT) {
	if (yearT==NULL) { yearT=year; }
	if (((yearT % 4) == 0) && ((yearT % 100) != 0) || ((yearT % 400) == 0)) {
		return true;
	} else {
		return false;
	}
}


//returns a timestamp giving the number of seconds since a part year (default=1970)
unsigned long swRTC::getTimestamp(int yearT){
	unsigned long time=0;

	//check the epoch
	if (yearT==NULL) {
		yearT=1970;
	} else if (yearT<1900) { 
		yearT=1900;
	} else if (yearT>1970) {
		yearT=1970;
	} else if ((yearT!=1900) && (yearT!=1970)) {
		yearT=1970;
	}
	
	//One revolution of the Earth is not 365 days but accurately 365.2422 days. 
	//It is leap year that adjusts this decimal fraction. But...
	time += (getYear()-yearT)*365.2422;
	for (int i=0; i < getMonth()-1; i++){
		time += daysPerMonth[i]; //find day from month
	}
	time = ( time + getDay() ) * 24; //find hours from day
	time = ( time + getHours() ) * 60; //find minutes from hours
	time = ( time + getMinutes() ) * 60; //find seconds from minute
	time += getSeconds(); // add seconds
	if (time > 951847199UL) { time += 86400UL; } //year 2000 is a special leap year, so 1 day must be added if date is greater than 29/02/2000
	return (time - 86400UL); //because years start at day 0.0, not day 1.
}


//set deltaT to correct the deviation between computed & real time
//(float, given as seconds per day)
boolean swRTC::setDeltaT(float deltaT) {
	if ((deltaT<-840.0) || (deltaT>840.0)) {
		return false;
	}
	delta=deltaT*10;
	if (delta==0) {
		deltaS=0;
		deltaDir=0;
	} else {
		deltaS=abs(864000/delta);
		deltaDir=delta/(abs(delta));
		delta=abs(delta);
	}
	return true;
}

//set deltaT to correct the deviation between computed & real time
//(integer, given as tenths of seconds per day)
boolean swRTC::setDeltaT(int deltaT) {
	if ((deltaT<-8400) || (deltaT>8400)) {
		return false;
	}
	delta=deltaT;
	if (delta==0) {
		deltaS=0;
		deltaDir=0;
	} else {
		deltaS=abs(864000/delta);
		deltaDir=delta/(abs(delta));
		delta=abs(delta);
	}
	return true;
}

// set the internal clock using a timestamp using the epoch passed as argument
byte swRTC::setClockWithTimestamp(unsigned long timeT, int yearRef) {
	unsigned long dayT;

	if (timeT > 951847199UL) { timeT -= 86400UL; } //year 2000 is a special leap year, so 1 day must be added if date is greater than 29/02/2000
	timeT += 86400; //days in the calendar start from Jan., 1 not from Jan., 0
	dayT = timeT/(60L*60L*24L);
	float remaining = timeT-dayT*(60L*60L*24L);
	int yearT = (dayT / 365.2422);
	float dayRemaining = dayT-yearT*365.2422;
	
	if (yearRef==NULL) {
		yearRef=1970;
	} else if (yearRef<1900) {
		yearRef=1900;
	} else if (yearRef>1970) {
		yearRef=1970;
	} else if ((yearRef!=1900) && (yearRef!=1970)) {
		yearRef=1970;
	}
	
	yearT+=yearRef;
	if (dayRemaining>=365.2422)
		return 1;//my math is wrong!
	if (yearT<yearRef)
		return 2;//year not supported!
	int monthT=0;
	while (dayRemaining > daysPerMonth[monthT]){
		dayRemaining -= daysPerMonth[monthT];
		if (monthT==1 && isLeapYear(yearT)) {
			dayRemaining--;
		}
		monthT++;
	}

	monthT++;//because month 0 doesn't exist
	if (monthT>12)
		return 3;//my math is wrong!
	if (dayRemaining>=(60L*60L*24L))
		return 4;//my math is wrong!
	dayT=dayRemaining;
	if (dayRemaining-dayT>0){ //add partial day!
		dayT++;
	}
	int hoursT = remaining/(60L*60L);
	remaining = remaining-hoursT*(60L*60L);
	if (remaining>=(60L*60L))
		return 5;//my math is wrong!
	int minutesT = remaining/(60L);
	remaining = remaining-minutesT*(60L);
	if (remaining>=60)
		return 6;//my math is wrong!

	year=yearT;
	month = monthT;
	day = dayT;
	hours = hoursT;
	minutes = minutesT;
	secondsX = remaining;
	return 0;
}


//return the day of the week giving a timestamp
byte swRTC::weekDay(unsigned long timestamp) {
  return ((timestamp / 86400UL) + 4 ) % 7; 
}


//return the current week day
byte swRTC::getWeekDay() {
	return (swRTC::weekDay(swRTC::getTimestamp()));
}

