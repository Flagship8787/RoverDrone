#ifndef RoverController_h
#define RoverController_h

#include "Arduino.h"

#include <SoftwareSerial.h>

#include <RoverMonitor.h>
#include <RoverControlClient.h>

#define kXBeeRx 6
#define kXBeeTx 7

#define kCommandInterval (1000 / 12)

#define kBufferCapacity 300

class RoverController
{
	public:
		static RoverController* SharedInstance();

		RoverController();
		void setupController();
		void controlRover();

	private:
		//	The XBee
		//SoftwareSerial xbee;

		//	Monitor the Rover
		char buffer[kBufferCapacity];
		int bufferLength;

		void flushBuffer();
		void monitorRover();

		//	Sending commands from the controller
		unsigned int LR;
		unsigned int UD;

		unsigned long lastCommand;

		void sendControlCommands();
};

#endif