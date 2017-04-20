#include "variableHeader.h";

//////////////////////// Colors
void White()
{
  analogWrite(RPin, 0);
  analogWrite(GPin, 0);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  analogWrite(BPin, 0);
}

void Off()
{
  analogWrite(RPin, 700);
  analogWrite(GPin, 700);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  analogWrite(BPin, 700);
}


void Red()
{
  analogWrite(RPin, 0);
  analogWrite(GPin, 1000);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  analogWrite(BPin, 1000);
}


void Cyan()
{
  analogWrite(RPin, 1000);
  analogWrite(GPin, 0);
  analogWrite(BPin, 0);
}


/////////////////////////////////// Alerts:
void EmergencyAlert()
{
  Red();
  delay(200);
  White();
  delay(200);
}

void NormalAlert() // transient between red and white
{
  analogWrite(RPin, 0);
  for(int x = 0; x < 1000; x++)
  {
    analogWrite(GPin, x);
    analogWrite(BPin, x);
    delay(1);
  }

  for(int x = 1000; x >= 0; x--)
  {
    analogWrite(GPin, x);
    analogWrite(BPin, x);
    delay(1);
  }
  
}

void RomanticAlert() // heart beat in Cyan
{
  Off();
  delay(100);
  Cyan();
  delay(150);
  Off();
  delay(100);
  Cyan();
  delay(800);
}

