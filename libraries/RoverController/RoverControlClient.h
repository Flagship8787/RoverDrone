#ifndef RoverControlClient_h
#define RoverControlClient_h

#include "Arduino.h"

//#include <SD.h>

#include <SPI.h>
#include <Ethernet.h>

#define kAppPort 3000

#define kIsDebugging false

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
		//void reportImageDataToServer();

	private:
		RoverControlClient();
		
		byte mac[6];
		RCNetworkStatus netStat;

		IPAddress server;
		EthernetClient client;
};

#endif