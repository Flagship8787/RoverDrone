#include "GPSManager.h"


GPSManager* GPSManager::SharedInstance()
{
	static GPSManager gpsInstance;
	return &gpsInstance;
}

GPSManager::GPSManager()
{
	flushBuffers();
	Serial1.begin(4800);
}

void GPSManager::flushBuffers()
{
	flushInBuffer();
	flushCmdBuffer();
}

void GPSManager::flushInBuffer()
{
	for (int i=0;i<kBufferCapacity;i++){
    	inBuffer[i]=' ';             
    }
    inBufferLength = 0;
}

void GPSManager::flushCmdBuffer()
{
	for (int i=0;i<kBufferCapacity;i++){
    	cmdBuffer[i]=' ';             
    }
    cmdBufferLength = 0;
}

void GPSManager::manageGPS()
{
	int gpsByte = Serial1.read();

	//	If there's no data, we return
	if(gpsByte == -1){			
		return;
	}

	//	If we get an ending character, save the current in buffer as a command and return
	if(gpsByte == 13){	
		/*
		Serial.println();		
		Serial.println("Begin GPS Command");

		for(int i = 0 ; i < inBufferLength ; i++){
			Serial.write(inBuffer[i]);
		}

		Serial.println("End GPS Command");
		*/
		saveInBufferAsCommand();
		flushInBuffer();
	
		return;
	}

	// If we get a beginning character we start the buffer anew
	if(gpsByte == '$'){			
		flushInBuffer();
	}

	//Serial.write(gpsByte);

	//	Now just buffer the new byte
	inBuffer[inBufferLength] = gpsByte;
	inBufferLength++;
}

void GPSManager::saveInBufferAsCommand()
{
	for(int i = 0 ; i < 6 ; i++){
		if((inBuffer[i] != kCommandoGPR[i])/* && (inBuffer[i] != kFixDataKey[i])*/){
			return;
		}
	}

	flushCmdBuffer();

	cmdBufferLength = inBufferLength;
	for(int i = 0 ; i < inBufferLength ; i++){
		cmdBuffer[i] = inBuffer[i];
	}
}