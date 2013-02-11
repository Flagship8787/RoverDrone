#ifndef RoverUplink_h
#define RoverUplink_h

#define kStatusReportInterval 5000

#include "Arduino.h"

#include <WProgram.h>

#include <GPSManager.h>
#include <RoverDriver.h>

#define kXBeeRx 8
#define kXBeeTx 9

class RoverUplink
{
	public:
		RoverUplink();
		void uplink();

	private:
		unsigned long lastStatusReport;
		//SoftwareSerial xbee;
};

#endif