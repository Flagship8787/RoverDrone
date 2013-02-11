#include "RoverController.h"

static RoverController rcInstance;
	
RoverController* RoverController::SharedInstance()
{
	return &rcInstance;
}

RoverController::RoverController()//: xbee(kXBeeRx, kXBeeTx, false)
{
	/*
	xbee.begin(9600);
	xbee.listen();
	*/

	lastCommand = 0; 
}

void RoverController::setupController()
{
	RoverControlClient *client = RoverControlClient::SharedInstance();
	client -> connectToNetwork();
}

void RoverController::controlRover()
{
	monitorRover();
	sendControlCommands();
}

//	Command the rover
void RoverController::sendControlCommands()
{
	if(millis() - lastCommand < kCommandInterval){
		return;
	}

	lastCommand = millis();

	LR = analogRead(A1);
	UD = analogRead(A2);

	Serial.print("$");
	Serial.print(LR);
	Serial.print(",");
	Serial.print(UD);
	Serial.print("@");
}

/******
 *	Receive Telemetry from the rover
 *****/
void RoverController::flushBuffer()
{
	for(int i = 0 ; i < kBufferCapacity ; i++){
		buffer[i] = ' ';
	}
	bufferLength = 0;
}

void RoverController::monitorRover()
{
	int aByte = Serial.read();

	if(aByte == -1){
		return;
	}

	if(aByte == '@'){
		RoverControlClient *client = RoverControlClient::SharedInstance();
		client -> reportTelemetryToServer(buffer, bufferLength);

		flushBuffer();

		return;
	}

	if(aByte == '$'){
		flushBuffer();
	}

	buffer[bufferLength] = aByte;
	bufferLength++;

	//Serial.write(aByte);
}
