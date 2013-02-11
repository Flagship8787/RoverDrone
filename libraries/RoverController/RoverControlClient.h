#ifndef RoverControlClient_h
#define RoverControlClient_h

#include "Arduino.h"

#include <SD.h>

#include <SPI.h>
#include <Ethernet.h>

#define kIsDebugging false

#define kAppPort 3000
#define kRoverIdFile "rover.id"

typedef enum{
	RCNetworkDisconnected,
	RCNetworkConnected
}RCNetworkStatus;

class RoverControlClient
{
	public:
  		static RoverControlClient* SharedInstance();

		void connectToNetwork();
		void reportTelemetryToServer(char[300],int);

	private:
		RoverControlClient();
		
		byte mac[6];
		RCNetworkStatus netStat;

		IPAddress server;
		EthernetClient client;

};

#endif