#ifndef RoverDriver_h
#define RoverDriver_h

#include "Arduino.h"

#include <WProgram.h>

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

#define kStopCommand "$STOP"
#define kStopCommandLength 5
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
		void fullStop();
		
		//	Ping stuff
		unsigned long lastPing;
		long distanceInFront();

		//	Motor Stuff
		unsigned long lastCommand;

		char cmdBuffer[kCmdBufferCapacity];
		int cmdBufferLength;

		void flushCmdBuffer();
		void receiveCommands();
		void parseCommand();
		boolean receivedStopCommand();
		void performDriveCommand(int, int);
};

#endif