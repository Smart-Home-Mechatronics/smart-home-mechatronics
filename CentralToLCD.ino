#include <ESP8266HTTPClient.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <ESP8266WiFi.h>
#include <DebugMacros.h>
#include <HTTPSRedirect.h>

#define ONE_WIRE_BUS 2 

extern "C" {
#include <cont.h>
  extern cont_t g_cont;
}

int PIR = 0;
int setTemperature=0;
int thermostatMode=0;
int out1=0;
int out2=0;
int noteStatus=0;
float tempC=0;
//String payload_base =  "{\"writeActualTemperature\": \"appendRow\", \"writeMotionDetection\": \"bullshit\"}";


OneWire oneWire(D4); 
DallasTemperature sensors(&oneWire);

// Network Credentials for Georgia Tech Wireless Network
// Choose GTother for NodeMCU, Raspberry Pi, or other wifi-enabled 
// Microcontroller/Microprocessor Units

const char* ssid = "GTother";
const char* password = "GeorgeP@1927";

const char* host = "script.google.com";
// Replace with your own script id to make server side changes
const char *GScriptId = "AKfycbxI7o_wVNI_aY59fME9SLSCVcpwqY--vIeXG7G6hj2AXEba_YgF";

const char* mac_add = "A0:20:A6:17:F8:D1";

const int httpsPort = 443;

// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
const char* fingerprint = "E4:7B:1F:46:3C:3E:E8:98:F1:4B:41:A5:AF:85:19:7D:E6:2A:73:DE";

// Write to Google Spreadsheet
String url = String("/macros/s/") + GScriptId + "/exec?";
//String url = String("/macros/s/") + GScriptId + "/exec?";



HTTPSRedirect* client = NULL;
// used to store the values of free stack and heap
// before the HTTPSRedirect object is instantiated
// so that they can be written to Google sheets
// upon instantiation
unsigned int free_heap_before = 0;
unsigned int free_stack_before = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(D5, INPUT);
  pinMode(D6, OUTPUT);  // COOLING
  pinMode(D7, OUTPUT);  // HEATING
  attachInterrupt(digitalPinToInterrupt(D5),PIRalert, RISING);

  digitalWrite(D6, HIGH);   // HIGH is OFF, LOW is ON
  digitalWrite(D7, HIGH);   // HIGH is OFF, LOW is ON
  Serial.begin(115200);
  Serial.flush();
    
  Serial.println();
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  // flush() is needed to print the above (connecting...) message reliably, 
  // in case the wireless connection doesn't go through
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
// Use HTTPSRedirect class to create a new TLS connection
  HTTPSRedirect* client = new HTTPSRedirect(httpsPort);
  client->setPrintResponseBody(true);
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){
    int retval = client->connect(host, httpsPort);
    if (retval == 1) {
       flag = true;
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  
  if (client->verify(fingerprint, host)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  }

  //payload = payload_base + "\"" + "abcd" + "\"}"; 
  client->GET(url+"writeActualTemperature=abcde",host);
  delete client;
  client = NULL;
}

void loop() {
  delay(1000);
  
  //Serial.println("LCD,"+ String(setTemperature) + "," + String(tempC) + "," + String(thermostatMode) + "," + String(PIR) + "," + String(noteStatus));
  
  //Serial.println("Send data to Google Script.");
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);
  //Serial.print("Temperature is: ");
  //Serial.println(tempC);

  client = new HTTPSRedirect(httpsPort);
  while (!client->connected()) {
    //Serial.print(".");
    client->connect(host, httpsPort);
  }
  //Serial.println();

  //Serial.println("payload to be sent: "+ String(val) + "," + String(tempC) + ",0,1");

  if(client->GET(url+"&writePIRActualreadSetTempModeOutput1Output2="+String(PIR) + ","+String(tempC) + ",0,1",host)) 
  {
    // if(client->GET(url+"writeActualTemperature="+String(tempC),host)) {
  
    String receivedData = client->getResponseBody();
    //Serial.println("Response body: " + receivedData);
    //SetTempModeOutput1Output2,67,2,0,0
    int firstCommaIndex = receivedData.indexOf(',');
    int secondCommaIndex = receivedData.indexOf(',', firstCommaIndex+1);
    int thirdCommaIndex = receivedData.indexOf(',', secondCommaIndex+1);
    int fourthCommaIndex = receivedData.indexOf(',', thirdCommaIndex+1);
    int fifthCommaIndex = receivedData.indexOf(',', fourthCommaIndex+1);
    setTemperature = receivedData.substring(firstCommaIndex+1, secondCommaIndex).toInt();
    thermostatMode = receivedData.substring(secondCommaIndex+1, thirdCommaIndex).toInt();
    out1 = receivedData.substring(thirdCommaIndex+1, fourthCommaIndex).toInt();
    out2 = receivedData.substring(fourthCommaIndex+1, fifthCommaIndex).toInt();
    noteStatus = receivedData.substring(fifthCommaIndex+1).toInt();
    //Serial.println("Set Temperature: "+String(setTemperature));
    //Serial.println("Thermostat Mode: "+ String(thermostatMode));
    //Serial.println("Output 1: "+String(out1));
    //Serial.println("Output 2: "+String(out2));
    //Serial.println("Notification Status: "+String(noteStatus));
////    
//    Serial.println(firstCommaIndex);
//    Serial.println(secondCommaIndex);
//    Serial.println(thirdCommaIndex);
//    Serial.println(fourthCommaIndex);
    
    //setTemperature = setTempString.substring(setTempString.length()-3).toInt();
    //Serial.println(setTemperature);
    //Serial.println("Successfully uploaded values.");
    //Serial.println(tempC);
    //Serial.println();
    // Light the LED to show that the GET request has been completed
    // For nodeMCU, LED LOW means light ON
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    //Serial.println("Upload failed.");
  }
  // AUTO
  if (thermostatMode == 0) {
    if (tempC < setTemperature - 1) {
      // TURN ON HEAT
      digitalWrite(D7, HIGH);
      digitalWrite(D6, LOW);  
    }
    else if(tempC > setTemperature + 1 ) {
      // TURN ON COOLING
      digitalWrite(D7, LOW);
      digitalWrite(D6, LOW);
    }
    else
    {
      digitalWrite(D7, HIGH);
      digitalWrite(D6, HIGH);
    }
  }
  // HEATING
  else if (thermostatMode == 1) {
    if (tempC < setTemperature) {
      digitalWrite(D6, LOW);
      digitalWrite(D7, HIGH);
    }
    else {
      digitalWrite(D6, HIGH);
      digitalWrite(D7, HIGH);
    }
  }
  // COOLING
  else if (thermostatMode == 2) {
    if (tempC > setTemperature) {
      digitalWrite(D7, LOW);
      digitalWrite(D6, LOW);
    }
    else {
      digitalWrite(D6, HIGH);
      digitalWrite(D7, HIGH);
    }
  }
  // OFF
  else
  {
    digitalWrite(D7, HIGH);
    digitalWrite(D6, HIGH);
  }
  
  delay(1000);
  //LED off
  digitalWrite(LED_BUILTIN, HIGH);
  


  delete client;
  client = NULL; 
  
//  delay(2000);
  Serial.println("LCD,"+ String(setTemperature) + "," + String(tempC) + "," + String(thermostatMode) + "," + String(PIR) + "," + String(noteStatus));
  Serial.println();
  PIR = 0;

}

void PIRalert()
{
  // LED ON
  PIR = 1;
  Serial.println("PIR Alert: " + String(PIR));
}

