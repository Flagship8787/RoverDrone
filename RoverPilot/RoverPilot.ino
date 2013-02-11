#include <SoftwareSerial.h>
#include <RoverPilot.h>

RoverPilot pilot;

int pingPin = 4;

void setup()
{
  Serial.begin(9600);
   
  while(!Serial){
    ;
  }
}

void loop()
{
  pilot.pilot();
}
