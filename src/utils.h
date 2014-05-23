#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <math.h>
#include <Arduino.h>
#include <TimerOne.h>

/*
************************************************************************************************************************
*           Actuator Types
************************************************************************************************************************
*/

#define ACT_TYPE_ABS	0x00
#define ACT_TYPE_INC	0x01

/*
************************************************************************************************************************
*           Mask Properties
************************************************************************************************************************
*/

#define TAP_TEMPO		0X02    //Masks
#define ENUMERATION		0X04
#define SCALE_POINTS	0X08
#define TRIGGER			0X10
#define TOGGLED			0X20
#define LOGARITHMIC		0X40
#define INTEGER			0X80

/*
************************************************************************************************************************
*           Time Defines
************************************************************************************************************************
*/

// in milisseconds

#define POLLING_PERIOD				2
#define DEVICE_TIMEOUT_PERIOD		32*POLLING_PERIOD
#define RANDOM_CONNECT_RANGE_BOTTOM	32
#define RANDOM_CONNECT_RANGE_TOP	320
/*
************************************************************************************************************************
*           Transmission defines
************************************************************************************************************************
*/

#ifndef MAX_DATA_SIZE
#define MAX_DATA_SIZE   200 //States how many bytes a message can contain. If trespass that, message will be discarded.
#endif

#define HOST_ADDRESS	0x00

#define WRITE_READ_PIN 	2
#define WRITE_ENABLE 	HIGH
#define READ_ENABLE 	LOW

// #define BAUD_RATE       300
// #define BAUD_RATE       1200
// #define BAUD_RATE       2400
// #define BAUD_RATE       4800
// #define BAUD_RATE       9600
// #define BAUD_RATE       14400
// #define BAUD_RATE       19200
// #define BAUD_RATE       28800
// #define BAUD_RATE       38400
// #define BAUD_RATE       57600
// #define BAUD_RATE       115200
// #define BAUD_RATE       230400
#define BAUD_RATE       500000

/*
************************************************************************************************************************
*           Protocol wise
************************************************************************************************************************
*/

#define FUNC_CONNECTION				0x01
#define FUNC_DEVICE_DESCRIPTOR		0x02
#define FUNC_CONTROL_ADDRESSING		0x03
#define FUNC_DATA_REQUEST			0x04
#define FUNC_CONTROL_UNADDRESSING	0x05	
#define FUNC_ERROR					0xFF	

#define PROTOCOL_VERSION_BYTE1		0x01
#define PROTOCOL_VERSION_BYTE2		0x00	

#define BYTE_SYNC					'\xAA'
#define BYTE_ESCAPE					'\x1B'

/*
************************************************************************************************************************
*           Settings
************************************************************************************************************************
*/

#define USER_LED 13
#define CONNECTING_LED_PERIOD 500 // milisseconds

/*
************************************************************************************************************************
*           Other Stuff
************************************************************************************************************************
*/

#ifndef DEBUG_FLAG
#define DEBUG_FLAG		1
#endif

#define EPSLON   0.01

/*
************************************************************************************************************************
*           MACROS
************************************************************************************************************************
*/


// serial related MACROS


#ifndef SWRITE(__char) // sends one byte 
#define SWRITE(__char) Serial.write(__char)
#endif

#ifndef SREAD() //returns next byte in serial's input buffer
#define SREAD() Serial.read()
#endif

#ifndef SBYTESAVAILABLE() //returns how many bytes there are avaible at the serial's input buffer 
#define SBYTESAVAILABLE() Serial.available()
#endif

#ifndef SFLUSH() // Waits for the transmission of outgoing serial data to complete. If the bytes are sent imediately after the sWrite func, then this is useless
#define SFLUSH() Serial.flush()
#endif

#ifndef PRINT(__str) // normal printing
#define PRINT(__str) Serial.print(__str)
#endif

// debug msgs

#ifndef DPRINT(__str) // debug printing
#define DPRINT(__str) if(DEBUG_FLAG) PRINT(__str)
#endif

#ifndef ERROR(__str) // error msg
#define ERROR(__str) if(DEBUG_FLAG) {PRINT(F("[ERROR]: << ")); PRINT(F(__str)); PRINT(F(" >> "));} 
#endif

#ifndef WARN(__str) // warning msg
#define WARN(__str) if(DEBUG_FLAG) {PRINT(F("[WARNING]: << ")); PRINT(F(__str)); PRINT(F(" >> "));} 
#endif

/*
************************************************************************************************************************
*           Enumerations
************************************************************************************************************************
*/

enum{POS_SYNC, POS_DEST, POS_ORIG, POS_FUNC, POS_DATA_SIZE1, POS_DATA_SIZE2, NOT_USABLE_CHECKSUM, HEADER_SIZE};
enum{CONNECTING = 1, WAITING_DESCRIPTOR_REQUEST, WAITING_CONTROL_ADDRESSING, WAITING_DATA_REQUEST};
enum{DESTINATION = 1, ORIGIN};

/*
************************************************************************************************************************
*           Unions and Structs
************************************************************************************************************************
*/

union Value{
	float f;
	char c[4];
};

union u_Word{
	uint16_t data16;
	uint8_t data8[2];
};

union Word{
	int16_t data16;
	int8_t data8[2];
};

class Str{
public:
	char* msg;
	int length;

	Str(char* msg){ //case it's a normal string like "prefiro o antigo protocolo"
		int i = 0;

		for (; msg[i] != '\0'; ++i);
	
		this->length = i;

		this->msg = new char[this->length];

		for(i = 0; i < this->length; i++){
			this->msg[i] = msg[i];
		}
	}

	Str(char* msg, int length): length(length){ // case it's a serial msg, which includes \0 inside its content
		this->msg = new char[length];

		for(int i = 0; i < length; i++){
			this->msg[i] = msg[i];
		}
	}

	~Str(){
		delete[] this->msg;
	}

	bool operator==(const Str &other) const {
		if(this->length == other.length){
			for(int i = 0; i < this->length; i++){
				if(this->msg[i] != other.msg[i])
					return false;
			}
				return true;
		}
		else
			return false;

	}

	Str & operator=(const char* &msg){
		int i = 0;

		for (; msg[i] != '\0'; ++i);
	
		this->length = i;

		this->msg = new char[this->length];

		for(i = 0; i < this->length; i++){
			this->msg[i] = msg[i];
		}
	}

	Str & operator=(const Str &msg){
		this->length = msg.length;

		this->msg = new char[this->length];

		for(int i = 0; i < this->length; i++){
			this->msg[i] = msg.msg[i];
		}
	}
};



struct STimer
{
	uint16_t period;
	uint16_t counter;
	bool working;
	bool timeout_flag;

	STimer(int period){ //period in milisseconds
		this->period = period;
		this->counter = 0;
		this->working = false;
		this->timeout_flag = false;
	}

	void increment(){
		if(working){
			if(counter >= period){
				timeout_flag = true;
				counter = 0;
			}
			else{
				counter++;
			}
			
		}
		else return;
	}

	void setPeriod(uint32_t period){
		if(working){
			this->period = period;
			counter = 0;
			timeout_flag = false;
		}
		else{
			return;
		}
	}

	bool check(){ // tells if the timer reached its period
		if(working && timeout_flag){
			return true;
		}
		else
			return false;
	}

	uint32_t consult(){
		return counter;
	}

	void stop(){
		working = false;
		timeout_flag = false;
		counter = 0;
	}

	void start(){
		working = true;
	}

	void reset(){
		timeout_flag = false;
		counter = 0;
	}
};


/**************************************************************************************
                                    Instantiations
***************************************************************************************/

STimer timerA(1);
STimer timerB(1);
STimer timerSERIAL(1);
STimer timerLED(1);

// Msg message;

/**************************************************************************************
                                    Functions
***************************************************************************************/


int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void send(char byte){ // this function sends bytes via swrite
	if(byte == BYTE_SYNC){
		SWRITE(BYTE_ESCAPE);
		SWRITE(~byte);
	}
	else{
		SWRITE(byte);
	}
}

void send(char* msg, int length){ //same thing for strings
	for (int i = 0; i < length; ++i){
		send(msg[i]);		
	}
}

unsigned char checkSum(char* msg, int length){
	unsigned char checksum;

	for (int i = 0; i < length; ++i){
		checksum += msg[i];
	}

	return checksum;
}

void isr_timer(){
	if(timerA.working)
		timerA.increment();
	if(timerB.working)
		timerB.increment();
	if(timerLED.working)
		timerLED.increment();
	if(timerSERIAL.working)
		timerSERIAL.increment();
}

void initializeDevice(){
	timerA.start();
	Timer1.initialize(1000);
	Timer1.attachInterrupt(isr_timer);
}

#endif