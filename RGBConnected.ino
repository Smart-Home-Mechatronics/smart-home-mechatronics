#include "ESP8266WiFi.h"
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "HTTPSRedirect.h"
#include "variableHeader.h"

WiFiServer server(80);

const char* ssid = "GTother";
const char* password = "GeorgeP@1927";
String st;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";
const char* GScriptId = "AKfycbxI7o_wVNI_aY59fME9SLSCVcpwqY--vIeXG7G6hj2AXEba_YgF";
const int httpsPort = 443;

///////////////////////////// Libraries and variables declearation for the sensors: \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15
#define D9 3 
#define D10 1


void setup() 
{   

  pinMode(RPin, OUTPUT);   // sets the pin as output
  pinMode(GPin, OUTPUT);   // sets the pin as output
  pinMode(BPin, OUTPUT);   // sets the pin as output  
  
  Serial.begin(115200);

  // Use HTTPSRedirect class to create TLS connection (for GoogleSheets)
  HTTPSRedirect client(httpsPort);  
 
  Serial.println();  
  Serial.println("Mac address: " + WiFi.macAddress());
}

void loop()
{
  for(int i = 0; i < 20; i++) // only read the data from the cloud once per 20
  {
    if(i==0)
    {
      readNotificationStatus();
    }
    switch(notificationStatus)
    {
      case 0:
        White();
        break;
      case 1:
        RomanticAlert();
        break;
      case 2:
        NormalAlert();
        break;
      case 3:
        EmergencyAlert();
        break;
    }
  }
  
}


void readNotificationStatus()
{
  HTTPSRedirect client(httpsPort);
  if(!client.connected())
  {
    client.connect(host,httpsPort);
    Serial.println("Reconnecting...");
  }
  String url = String("/macros/s/") + GScriptId + "/exec?readNotificationStatus";
  Serial.println("URL" + url);
  client.printRedir(url, host, googleRedirHost);
}

