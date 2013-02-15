#include "RoverController.h"

static RoverController rcInstance;
	
RoverController* RoverController::SharedInstance()
{
	return &rcInstance;
}

RoverController::RoverController()//: imageFile(), xbee(kXBeeRx, kXBeeTx, false)
{
	/*
	xbee.begin(9600);
	xbee.listen();
	*/

	//	Image Button Setup
	/*
	pinMode(kLedPin, OUTPUT);
	pinMode(kButtonPin, INPUT);
	
	buttonPressed = false;
	waitingForImage = false;

	imageRequestTime = 0;
	imageDataReceived = 0;
	
	imgEndingChars = 0;
	*/

	//	Driving command constants
	LR = 0;
	UD = 0;
	
	lastCommand = 0; 
}

void RoverController::setupController()
{
	/*
	pinMode(10, OUTPUT);

	if (!SD.begin(4)) {
		Serial.println("initialization failed!");
		return;
	}
	*/
	
	//	Setup the network connection
	RoverControlClient *client = RoverControlClient::SharedInstance();
	client -> connectToNetwork();
}

void RoverController::controlRover()
{
	/*
	//	Handle downloading the image (if necessary)
	if(waitingForImage){
		digitalWrite(kLedPin, HIGH);
		receiveImageFromRover();
		
		return;
	}else{
		digitalWrite(kLedPin, LOW);
	}

	//	If the button was pressed but isn't now, then it's time to request an image
	int buttonState = digitalRead(kButtonPin);
	if(buttonPressed && buttonState == LOW){
		buttonPressed = false;
		requestImageFromRover();
		
		return;
	}
	
	if(buttonState == HIGH){
		buttonPressed = true;
	}
	*/
	
	//	If nothing else is going on, then just do the regular
	monitorRover();
	sendControlCommands();
}

/*****
 *	Command the rover
 ****/
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
}

/******
 *	Request and receive images from the server
 *****/
/*
void RoverController::requestImageFromRover()
{
	if(SD.exists("roverImg.jpg")){
		SD.remove("roverImg.jpg");
	}

	imageFile = SD.open("roverImg.jpg", FILE_WRITE);
	
	Serial.print(kImageCommand);
	
	waitingForImage = true;

	imageRequestTime = millis();
	imageDataReceived = millis();
	
	Serial.println("Waiting for image");
}

void RoverController::receiveImageFromRover()
{
	int imgByte = Serial.read();
	if(imgByte < 0){
		if(millis() - imageDataReceived > kImageRequestTimeout){
			imageFile.close();
			waitingForImage = false;
		}
		
		return;
	}
	
	imageDataReceived = millis();
	
	if(imgByte == '@'){
		imgEndingChars++;
		if(imgEndingChars == kImageEndingLength){
			imageFile.close();
			
			RoverControlClient *controlClient = RoverControlClient::SharedInstance();
			controlClient -> reportImageDataToServer();
			
			Serial.flush();
			flushBuffer();
			
			waitingForImage = false;
		}
		
		return;
	}
	
	//	If we've already received some ending characters in a row that means they were part of the image, so write them to the file
	if(imgEndingChars > 0){
		for(int i = 0 ; i < imgEndingChars ; i++){
			imageFile.write('@');
		}
		
		imgEndingChars = 0;
	}
	
	//Serial.write(imgByte);
	imageFile.write(imgByte);
}
*/