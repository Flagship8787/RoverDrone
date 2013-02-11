#include "RoverDriver.h"

RoverDriver* RoverDriver::SharedInstance()
{
	static RoverDriver rdInstance;
	return &rdInstance;
}

RoverDriver::RoverDriver()
{
	//	Initialize the Ultrasound stuff
	currentClearance = 0;
	lastPing = 0;
	
	//	Motor Stuff
	pinMode(kFrCtrlPin, OUTPUT);
	pinMode(kFrBrakePin, OUTPUT);

	pinMode(kBkCtrlPin, OUTPUT);
	pinMode(kBkBrakePin, OUTPUT);

	//	Command stuff
	cmdBufferLength = 0;
	flushCmdBuffer();

	lastCommand = 0;
}

void RoverDriver::drive()
{
	//	Check if we're gonna hit something
	unsigned long currentTime = millis();
	if(currentTime - lastPing > kPingInterval){
		currentClearance = distanceInFront();
		lastPing = currentTime;
		if(currentClearance < kMinClearance){
			fullStop();
			return;
		}
	}

	receiveCommands();
	if(millis() - lastCommand > kCommandMaxDowtime){
		fullStop();
	}
}

void RoverDriver::fullStop()
{
	digitalWrite(kFrBrakePin, HIGH);
	digitalWrite(kBkBrakePin, HIGH);
}

long RoverDriver::distanceInFront()
{
	long duration, cm;

  	// The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  	pinMode(kPingPin, OUTPUT);
  	digitalWrite(kPingPin, LOW);
  	delayMicroseconds(2);
  	digitalWrite(kPingPin, HIGH);
  	delayMicroseconds(5);
  	digitalWrite(kPingPin, LOW);

  	// The same pin is used to read the signal from the PING))): a HIGH
  	// pulse whose duration is the time (in microseconds) from the sending
  	// of the ping to the reception of its echo off of an object.
  	pinMode(kPingPin, INPUT);
  	duration = pulseIn(kPingPin, HIGH);

  	// The speed of sound is 340 m/s or 29 microseconds per centimeter.
  	// The ping travels out and back, so to find the distance of the
  	// object we take half of the distance travelled.
  	cm = duration / 29 / 2;
 	//delay(100);

 	return cm;
}

void RoverDriver::flushCmdBuffer()
{
	for (int i=0;i<kCmdBufferCapacity;i++){
    	cmdBuffer[i]=' ';             
    }
    cmdBufferLength = 0;
}

void RoverDriver::receiveCommands()
{
	int cmdByte = Serial.read();

	//	If there's no data, we return
	if(cmdByte == -1){			
		return;
	}

	//	If we get an ending character, save the current in buffer as a command and return
	if(cmdByte == '@'){	
		lastCommand = millis();

		parseCommand();
		flushCmdBuffer();
	
		return;
	}

	// If we get a beginning character we start the buffer anew
	if(cmdByte == '$'){			
		flushCmdBuffer();
	}

	//Serial.write(cmdByte);

	//	Now just buffer the new byte
	cmdBuffer[cmdBufferLength] = cmdByte;
	cmdBufferLength++;
}

void RoverDriver::parseCommand()
{

	//	Check for stop command
	if(receivedStopCommand()){
		fullStop();
		return;
	}

	//	Find the seperator
	int seperatorInd = 0;
	for(int i = 0 ; i < cmdBufferLength ; i++){
		if(cmdBuffer[i] == ','){
			seperatorInd = i;
		}
	}

	/****
	 *	NOTE ON ARRAY SIZES - the size of a substring form start a to end b (excluding the character at b) 
	 * 	has a length of (b - a).  Duh.
	 ****/

	//	Record the left/right param
	char lrParam[(seperatorInd - 1) + 1];
	for(int i = 1 ; i < seperatorInd ; i++){
		lrParam[i - 1] = cmdBuffer[i];
	}
	lrParam[seperatorInd - 1] = 0x00;

	//	Record the up/down param
	char fbParam[(cmdBufferLength - (seperatorInd + 1)) + 1];
	for(int i = (seperatorInd + 1) ; i < cmdBufferLength ; i++){
		fbParam[i - (seperatorInd + 1)] = cmdBuffer[i];
	}
	fbParam[cmdBufferLength - (seperatorInd + 1)] = 0x00;

	int LR = atol(lrParam);
	int FB = atol(fbParam);
	
	performDriveCommand(LR, FB);
}

boolean RoverDriver::receivedStopCommand()
{
	if(cmdBufferLength < kStopCommandLength){
		return false;
	}

	for(int i = 0 ; i < kStopCommandLength ; i++){
		if(cmdBuffer[i] != kStopCommand[i]){
			return false;
		}
	}

	return true;
}

void RoverDriver::performDriveCommand(int LR, int FB)
{
	boolean isLeft;
	int lrMotorPwr = 0;
	if(LR > 0 && LR < 512){				//	Is Left
		isLeft = true;
		lrMotorPwr = ((512 - LR) / 2);
	}else if(LR > 512 && LR <= 1024){	//	Is Right
		isLeft = false;
		lrMotorPwr = ((LR - 512) / 2);
	}

	boolean isForward;
	int fbMotorPwr = 0;
	if(FB > 0 && FB < 512){				//	Is Backward
		isForward = false;
		fbMotorPwr = ((512 - FB) / 2);
	}else if(FB > 512 && FB <= 1024){	//	Is Forward
		isForward = true;
		fbMotorPwr = ((FB - 512) / 2);
	}

	if(lrMotorPwr > kMinMotorPower){
		if(isLeft){
			digitalWrite(kFrCtrlPin, HIGH);
		}else{
			digitalWrite(kFrCtrlPin, LOW);
		}
		
		digitalWrite(kFrBrakePin, LOW);
		analogWrite(kFrPwrPin, lrMotorPwr);
	}else{
		digitalWrite(kFrBrakePin, HIGH);
	}
	
	if(fbMotorPwr > kMinMotorPower){
		if(isForward){
			digitalWrite(kBkCtrlPin, LOW);
		}else{
			digitalWrite(kBkCtrlPin, HIGH);
		}
		digitalWrite(kBkBrakePin, LOW);
		analogWrite(kBkPwrPin, fbMotorPwr);
	}else{
		digitalWrite(kBkBrakePin, HIGH);
	}
}