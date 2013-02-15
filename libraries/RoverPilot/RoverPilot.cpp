#include "RoverPilot.h"

RoverPilot::RoverPilot(): uplink()
{

}

void RoverPilot::pilot()
{
	GPSManager *gps = GPSManager::SharedInstance();
	gps -> manageGPS();

	RoverDriver *driver = RoverDriver::SharedInstance();
	driver -> drive();
	
	uplink.uplink();
}
