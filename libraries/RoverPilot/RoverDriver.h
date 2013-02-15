#ifndef RoverDriver_h
#define RoverDriver_h

#include "Arduino.h"

#include <WProgram.h>
//#include <RoverCamera.h>

#define kPingPin 7
#define kMinClearance 3
#define kPingInterval 250

#define kFrCtrlPin 12
#define kFrBrakePin 9
#define kFrPwrPin 3

#define kBkCtrlPin 13
#define kBkBrakePin 8
#define kBkPwrPin 11

#define kCmdBufferCapacity 15

//	Commands from the controller
#define kStopCommand "$STOP"
#define kStopCommandLength 5

#define kImageCommand "$IMG"
#define kImageCommandLength 4

#define kCommandMaxDowtime 1500
#define kMinMotorPower 50

class RoverDriver
{
	public:
		RoverDriver();
		static RoverDriver* SharedInstance();
		void drive();

		long currentClearance;

	private:
		//	Ping stuff
		unsigned long lastPing;
		long distanceInFront();

		//	Controller Command Stuff
		unsigned long lastCommand;

		char cmdBuffer[kCmdBufferCapacity];
		int cmdBufferLength;

		void flushCmdBuffer();
		
		void receiveCommands();
		void parseCommand();
		
		boolean receivedStopCommand();
		boolean receivedImageCommand();
		
		//	Driving
		void performDriveCommand(int, int);
		void fullStop();
		
		//	Images
		boolean takingPicture;
};

#endif