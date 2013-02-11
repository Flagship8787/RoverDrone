#include <RoverController.h>

#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

RoverController rControl;

void setup()  
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  rControl.setupController();
}

void loop()
{
  rControl.controlRover();
}

