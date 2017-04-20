/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341 Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#define SERIAL_RX_BUFFER_SIZE 64


#include "SPI.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

#define BOXSIZE 40

int currentPage = 0;  /// 0:Temp, 1:PIR, 2:Not
char inChar;
char inData[100];
int index = 0;
String incomingData = "";

////TEMP INSTANTIATE
String setTemp = "-99";
String actTemp = "-99.9";
String tempModeCurr = "N/A";

String setTempPrev = "-99";
String actTempPrev = "-99.9";
String tempModePrev = "N/A";

////PIR INSTANTIATE
String PIR = "N/A";
String prevPIR = "N/A";

////NOTIFICATION INSTANTIATE

String Not = "N/A";
String prevNot = "N/A";

int firstCommaIndex, secondCommaIndex, thirdCommaIndex, fourthCommaIndex, fifthCommaIndex;
String modeState, PIRState, noteState;
bool pageHasChanged = false;
String currStateArray[5] = {setTemp, actTemp, modeState, PIRState, noteState};
String prevStateArray[5];
String newStateArray[5];

void setup(void) { 
 // while (!Serial);     // used for leonardo debugging
 
  Serial.begin(115200);
  Serial.println("Touch Paint!");
  tft.begin();
  tft.setRotation(3);
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");
  tft.fillScreen(ILI9341_BLACK);
  // make the page selection boxes
  createArrows();
  mainFixed(ILI9341_YELLOW);
  mainUpdate(ILI9341_GREEN);
}


void loop() {
//  // See if there's any  touch data for us
//  
//////////////////////////READ DATA//////////////////////////////////////////////////////
//    Serial.println("reading data");
  incomingData = "";
  index = 0;
  while(Serial.available())
  {
    inChar = Serial.read();    
    inData[index] = inChar;
    index++;
    incomingData = inData;
    Serial.println(incomingData);
  }
    
  inData[index] = '\0'; // Null terminate the string
  incomingData = inData; 
  Serial.println(incomingData);
  //Serial.println(incomingData.length());

  
  // parse the data: 

  if((incomingData.length() > 15) && (incomingData.substring(0 , 3) == "LCD" || incomingData.substring(0 , 2) == "CD" || incomingData.substring(0 , 1) == "D"))
  {
    Serial.println(incomingData);
    firstCommaIndex = incomingData.indexOf(',');
    secondCommaIndex = incomingData.indexOf(',', firstCommaIndex + 1);
    thirdCommaIndex = incomingData.indexOf(',', secondCommaIndex + 1); 
    fourthCommaIndex = incomingData.indexOf(',', thirdCommaIndex + 1); 
    fifthCommaIndex = incomingData.indexOf(',', fourthCommaIndex + 1); 

    clearPage(currentPage,ILI9341_BLACK);
    
    setTemp  = incomingData.substring(firstCommaIndex + 1, secondCommaIndex) + (char)247 + "C";
    actTemp = incomingData.substring(secondCommaIndex + 1, thirdCommaIndex) + (char)247 + "C";
    modeState = incomingData.substring(thirdCommaIndex + 1, fourthCommaIndex); 
    PIRState = incomingData.substring(fourthCommaIndex + 1, fifthCommaIndex);
    noteState = incomingData.substring(fifthCommaIndex + 1, incomingData.length() - 1);

    switch (modeState.toInt()) {
      case 0:
        tempModeCurr = "Auto";
      break;
      case 1:
        tempModeCurr = "Heat";
      break;
      case 2:
        tempModeCurr = "Cool";
      break;
      default:
        tempModeCurr = "OFF";
      break;
    }
  
    switch (PIRState.toInt()) {
      case 0:
        PIR = "Idle";
      break;
      case 1:
        PIR = "Motion";
      break;
    }
  
    switch(noteState.toInt()) {
      case 0:
        Not = "No Alert";
        break;
      case 1:
        Not = "Romantic";
        break;
      case 2:
        Not = "Normal";
        break;
      case 3:
        Not = "Urgent";
        break;
    }
    clearPage(currentPage, ILI9341_GREEN);
    //changePage(currentPage, true);
  }

  // Prepare data

//  
//  
// /////////////////////////END   ///////////////////////////////////////////////////
  if (ts.touched()) {
    TS_Point p = ts.getPoint(); 
      while ( ! ts.bufferEmpty() ){ p = ts.getPoint();}


    // Scale from ~0->4000 to tft.width using the calibration #'s
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());


    /// 0:Main, 1:PIR, 2:Notes
    //////////PREVIOUS PAGE/////////////////////
    if (p.y > (tft.height() - BOXSIZE)) {
      currentPage -= 1;
      if (currentPage < 0) {
        currentPage+=3;
      }
      changePage(currentPage);
      Serial.println(currentPage);
    }

    else if (p.y < (BOXSIZE)) {
      currentPage += 1;
      if (currentPage > 2) {
        currentPage -=3;
      }
      changePage(currentPage);
      Serial.println(currentPage);
    }
  }
}
////END LOOP
void changePage(int currentPage) {
  switch (currentPage) {
    case 0:
      tft.fillScreen(ILI9341_BLACK);
      mainFixed(ILI9341_YELLOW);
      mainUpdate(ILI9341_GREEN);
      break;

    case 1:
      tft.fillScreen(ILI9341_BLACK);
      PIRFixed(ILI9341_YELLOW);
      PIRUpdate(ILI9341_GREEN);
      break;
    
    case 2:      
      tft.fillScreen(ILI9341_BLACK);
      noteFixed(ILI9341_YELLOW);
      noteUpdate(ILI9341_GREEN);
      break;
  }
  createArrows();
  while ( ! ts.bufferEmpty() ){
    TS_Point p = ts.getPoint();
   }
  delay(100);

}

void clearPage(int currentPage, uint16_t x) {
  switch (currentPage) {
    case 0:
      mainUpdate(x);
      break;
    case 1:
      PIRUpdate(x);
      break;
    case 2:
      noteUpdate(x);
      break;
  }
}

unsigned long mainFixed(uint16_t x)
{
  //tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(x); 
  
  tft.setCursor(5, 15);
  tft.setTextSize(3); 
  tft.println("Set Temp:");
  
  tft.setCursor(5, 90);
  tft.setTextSize(3); 
  tft.println("Current Temp:");
  
  tft.setCursor(5, 160);
  tft.setTextSize(3); 
  tft.println("Mode:");
}

unsigned long mainUpdate(uint16_t x) 
{
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(95, 45);
  tft.setTextSize(5); 
  tft.println(setTempPrev);
  
  tft.setCursor(95, 120);
  tft.setTextSize(5); 
  tft.println(actTempPrev);

  tft.setCursor(95, 190);
  tft.setTextSize(5); 
  tft.println(tempModePrev);

  
  tft.setTextColor(x); 
  
  tft.setCursor(95, 45);
  tft.setTextSize(5); 
  tft.println(setTemp);
  
  tft.setCursor(95, 120);
  tft.setTextSize(5); 
  tft.println(actTemp);

  tft.setCursor(95, 190);
  tft.setTextSize(5); 
  tft.println(tempModeCurr);

  setTempPrev = setTemp; 
  actTempPrev = actTemp;
  tempModePrev = tempModeCurr;
}
unsigned long PIRFixed(uint16_t x)
{
  tft.setTextColor(x); 
  
  tft.setCursor(0, 50);
  tft.setTextSize(4); 
  tft.println("PIR Status:");
  
}

unsigned long PIRUpdate(uint16_t x) 
{
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(55, 125);
  tft.setTextSize(6); 
  tft.println(prevPIR);

  tft.setTextColor(x); 
  tft.setCursor(55, 125);
  tft.setTextSize(6); 
  tft.println(PIR);

  prevPIR = PIR;
}

unsigned long noteFixed(uint16_t x)
{
  tft.setTextColor(x); 
  
  tft.setCursor(0, 50);
  tft.setTextSize(4); 
  tft.println("Notification:");
  
}

unsigned long noteUpdate(uint16_t x) 
{
  tft.setTextColor(ILI9341_BLACK);
  tft.setCursor(25, 125);
  tft.setTextSize(6); 
  tft.println(prevNot);

  tft.setTextColor(x); 
  tft.setCursor(25, 125);
  tft.setTextSize(6); 
  tft.println(Not);

  prevNot = Not;
}

unsigned long createArrows(){
  int i=10, cy = tft.height()-BOXSIZE/2, cx = tft.width();
  tft.fillRoundRect(0, cy-BOXSIZE/2, BOXSIZE, BOXSIZE, i, ILI9341_GREEN);
  tft.fillRoundRect(cx-BOXSIZE, cy-BOXSIZE/2, BOXSIZE, BOXSIZE, i/2, ILI9341_GREEN);
  tft.fillTriangle(i, cy, BOXSIZE - i, cy - i, BOXSIZE - i, cy + i,ILI9341_WHITE);
  tft.fillTriangle(cx-i, cy, (cx-BOXSIZE) + i, cy - i, cx-BOXSIZE + i, cy + i,ILI9341_WHITE);
}

