#ifndef GPSManager_h
#define GPSManager_h

#include "Arduino.h"

#include <WProgram.h>

#define kFixDataKey "$GPGGA"
#define kCommandoGPR "$GPRMC"
#define kBufferCapacity 300

class GPSManager
{
	public:
		static GPSManager* SharedInstance();
		GPSManager();
		void manageGPS();

		char cmdBuffer[kBufferCapacity];
		int cmdBufferLength;
		
	private:
		//	Buffer stuff
		char inBuffer[kBufferCapacity];
		int inBufferLength;

		void flushBuffers();
		void flushInBuffer();
		void flushCmdBuffer();

		void saveInBufferAsCommand();
};

#endif