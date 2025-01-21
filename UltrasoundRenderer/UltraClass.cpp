#include "Arduino.h"
#include <math.h>

#define ultraEcho 10
#define ultraTrigger 9

int distanceUltrasound(long intDuration);
int scanDistance();

void setupUltra()
{
  //Setup Ultrasound sensor
  pinMode(ultraEcho, INPUT); //Echo comes in to Device
  pinMode(ultraTrigger, OUTPUT); //Trigger is given out by Device
}

int measureDistance()
{
  //get 10 Distances and return more accurate value
  int testDistance;
  int sumDistance=0;
  for(int i = 0; i<10;i++)
  {
    testDistance=distanceUltrasound(scanDistance());
    
    if(testDistance<=3)
    {
      //invalid value, not counted
      i--; //try again
    }
    else if(testDistance>=400)
    {
      sumDistance=sumDistance+390; 
    }
    else
    {
      sumDistance=sumDistance+testDistance;
    }
  }
  //lets take the Avarage
  testDistance = sumDistance/10;

  return testDistance;
}

int scanDistance()
{
  noInterrupts(); //lets not interrupt echo sequence
  //returns distance
  //Clears the Trigger pin
  digitalWrite(ultraTrigger, LOW);
  delayMicroseconds(2);
  //Sending the Trigger
  digitalWrite(ultraTrigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultraTrigger, LOW);
  long echoDuration = pulseIn(ultraEcho, HIGH);
  interrupts(); //interrupts back on.
  
  return echoDuration;
}


int distanceUltrasound(long intDuration)
{
  //longDuration = longDuration / 48.816;
  intDuration = pow((long(intDuration)/48.816),0.97561);

  if(intDuration>400)
  {
    return 400;
  }
  else if(intDuration<0)
  {
    return 0;
  }
  return intDuration;
}
