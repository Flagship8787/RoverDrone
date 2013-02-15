#ifndef RoverController_h
#define RoverController_h

#include "Arduino.h"

//#include <SD.h>

#include <SPI.h>
#include <Ethernet.h>

#include <SoftwareSerial.h>

#include <RoverMonitor.h>
#include <RoverControlClient.h>

#define kXBeeRx 4
#define kXBeeTx 5

#define kImageCommand "$IMG@"

#define kImageEnding "@@@@@"
#define kImageEndingLength 5

#define kButtonPin 6
#define kLedPin 7

#define kImageRequestTimeout 6000

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
		
		//	Getting images from the rover
		/*
		boolean buttonPressed;
		boolean waitingForImage;
		
		unsigned long imageRequestTime;
		unsigned long imageDataReceived;

		int imgEndingChars;
		
		File imageFile;
		
		void requestImageFromRover();
		void receiveImageFromRover();
		*/

		//	Sending commands from the joystick
		unsigned int LR;
		unsigned int UD;

		unsigned long lastCommand;

		void sendControlCommands();
};

#endif