#include "RoverControlClient.h"

RoverControlClient* RoverControlClient::SharedInstance()
{
	static RoverControlClient ccInstance;
	return &ccInstance;
}

RoverControlClient::RoverControlClient():server(192,168,1,4), client()
{
	mac[0] = 0xDE;
  	mac[1] = 0xAD;
  	mac[2] = 0xBE;
  	mac[3] = 0xEF;
  	mac[4] = 0xFE;
  	mac[5] = 0xED;

  	netStat = RCNetworkDisconnected;

  	if(kIsDebugging){
    	Serial.println("RoverControlClient created");
  	}
}

void RoverControlClient::connectToNetwork()
{
	if(kIsDebugging){
    	Serial.println("RoverController attempting to connect to network");
  	}

	if (Ethernet.begin(mac) == 0) {
  		if(kIsDebugging){
      		Serial.println("Failed to configure Ethernet using DHCP");
    	}
  		// no point in carrying on, so do nothing forevermore:
  		for(;;)
    		;

    	netStat = RCNetworkDisconnected;

    	return;
	}

  	// print your local IP address:
  	if(kIsDebugging){
    	Serial.print("RoverController's connected to the network with IP: ");
    	Serial.println(Ethernet.localIP());
    	Serial.println();  
  	}
  
  	netStat = RCNetworkConnected;

  	// give the Ethernet shield a second to initialize:
	if(kIsDebugging){
    	Serial.println("Initializing connection...");
  	}

  	delay(1000);
}

void RoverControlClient::reportTelemetryToServer(char telemetry[300], int length)
{
	if(kIsDebugging){
    	Serial.println();
    	Serial.println("Reporting data to server:");
  	}
  
	if(!client.connect(server, kAppPort)){
    	if(kIsDebugging){
      		Serial.println("Failed to make a connection to the server");  
    	}

    	return;
  	}

  	if(kIsDebugging){
    	Serial.println("Connected.  Sending the datas.");
  	}
	
	client.print("POST /rovers/telemetry");
  	client.println(" HTTP/1.1");
  	client.println("Host: 192.168.1.4:3000");
  	client.println("Accept: text/html");
  	client.println("User-Agent: Arduino-Sensor");
  	client.println("Connection: keep-alive");
  	client.println("Content-Type: application/x-www-form-urlencoded");
  	client.print("Content-Length: ");
  	client.println(length);
  	client.println();

  	//  Start at index = 1 because the telemetry strings begin with '$'
  	for(int i = 1 ; i < length ; i++){
		client.print(telemetry[i]);
	}
  	client.println();

	//	Retreive the response
	if(!client.connected()){
    	delay(1000);
    	return;
  	}

  	if(kIsDebugging){
    	Serial.println("Server Says:");
  	}

  	while(client.connected()){
    	int inChar = client.read();
		
    	if(kIsDebugging){
      		Serial.write(inChar);
    	} 
 
    	if(inChar == '!'){
      		if(kIsDebugging){
        		Serial.println();
      		}
      		client.stop();
    	}
  	}
  
  	delay(1000);
}

/*
void RoverControlClient::reportImageDataToServer()
{
	if(!SD.exists("roverImg.jpg")){
		return;
	}
	
	if(kIsDebugging){
    	Serial.println();
    	Serial.println("Reporting image to server:");
  	}
  
	if(!client.connect(server, kAppPort)){
    	if(kIsDebugging){
      		Serial.println("Failed to make a connection to the server");  
    	}

    	return;
  	}

  	if(kIsDebugging){
    	Serial.println("Connected.  Sending the image.");
  	}
	
	File imageFile = SD.open("roverImg.jpg", FILE_READ);
	
	client.print("POST /rovers/image");
  	client.println(" HTTP/1.1");
  	client.println("Host: 192.168.1.4:3000");
  	client.println("Accept: text/html");
  	client.println("User-Agent: Arduino-Sensor");
  	client.println("Connection: keep-alive");
  	client.println("Content-Type: application/x-www-form-urlencoded");
  	client.print("Content-Length: ");
  	client.println((imageFile.size() + 4));
  	client.println();

  	//  Start at index = 1 because the telemetry strings begin with '$'
	client.print("img=");
  	while (imageFile.available()) {
		client.write(imageFile.read());
	}
  	client.println();

	imageFile.close();
	
	//	Retreive the response
	if(!client.connected()){
    	delay(1000);
    	return;
  	}

  	if(kIsDebugging){
    	Serial.println("Server Says:");
  	}

  	while(client.connected()){
    	int inChar = client.read();
		
    	if(kIsDebugging){
      		Serial.write(inChar);
    	} 
 
    	if(inChar == '!'){
      		if(kIsDebugging){
        		Serial.println();
      		}
      		client.stop();
    	}
  	}
}
*/