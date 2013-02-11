#include "RoverUplink.h"

RoverUplink::RoverUplink()
{
	lastStatusReport = millis();
}

void RoverUplink::uplink()
{
	unsigned long currentTime = millis();
	if(currentTime - lastStatusReport < kStatusReportInterval){
		return;
	}

	lastStatusReport = currentTime;

	String output = "$";

	output += "Name=rovershipone&Millis=";
	output += lastStatusReport;

	GPSManager *gps = GPSManager::SharedInstance();
	if(gps->cmdBufferLength > 0){
		output += "&GPSData=";
		int length = gps -> cmdBufferLength;

		for(int i = 0 ; i < length ; i++){
			char gpsByte = gps->cmdBuffer[i];
			if(gpsByte != '$'){
				output += gpsByte;
			}
		}
	}

	output += "&Clearance=";
	
	RoverDriver *driver = RoverDriver::SharedInstance();
	output += driver -> currentClearance;

	//Ending Character
	output += "@";

	for(int i = 0 ; i  < output.length() ; i++){
		Serial.write(output.charAt(i));
	}
	Serial.println();
}
