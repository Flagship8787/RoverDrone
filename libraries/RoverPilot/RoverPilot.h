#ifndef RoverPilot_h
#define RoverPilot_h

#include "Arduino.h"

#include <WProgram.h>
#include <GPSManager.h>
#include <RoverDriver.h>
#include <RoverUplink.h>

class RoverPilot
{
	public:
		RoverPilot();
		void pilot();

	private:
		RoverUplink uplink;
};

#endif