#define START_BUTTON 2
//Main program for Ultrasound Renderer

// StartButton's other pin is 4
//const uint8_t resetButton 8 // other pin is 7

//Pin 6 is Green with other Green wires, turns clockwise
//Pin 5 is White wire with other white wires, turns counter clockwise
//motor's orange on ground/voltage at green side of circuit board
//Motor's yellow on ground/voltage at white side

//Rotation
//delay(1500) is about half circle 180Degrees. Close enough for our test


#include "ScanClass.h"
#include "UltraClass.h"

volatile boolean startButtonPressed, startDone, waitProcessDraw;
boolean scanStarted,buttonDone;

//testingMode true  if testing for Serial
boolean testingMode =true;

//what can happen on what status
//Status 0 = Nothing happening, only looking for buttons
//status 1 = scanning
//status 2 = drawing
//status 99 = special testing
int statusInt = 0;


void setup() {
  startButtonPressed, scanStarted=false;
  buttonDone=true;

  //startButton is high, when not pressed and low, when pressed
  pinMode(START_BUTTON, INPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(START_BUTTON, HIGH);

  attachInterrupt(digitalPinToInterrupt(START_BUTTON), detectStartButton, CHANGE);

  setupUltra();

  startSerial(testingMode);

}
//Test void loop
//To test Gyro... call testGyroRotation(); from ScanClass

void loop()
{
  if(statusInt == 99)
  {
    delay(6000); //delay before testing
    //special testing if needed
    int testInt = measureDistance();
    
    statusInt =88; //do only once
  }
  if(startButtonPressed &&!buttonDone)
  {
    if(statusInt == 0)
    {
      statusInt = 1;
    }
    buttonDone = true;
  }
    
  if(statusInt == 1 && startButtonPressed && !scanStarted)
  {
    if(testingMode)
    {
      Serial.println("Start Scan");
      delay(1);
    }
    
    statusInt = scanProgram();
    scanStarted = true;
  }

  if(statusInt == 2)
  {
     if(testingMode)
    {
      Serial.println("Start Drawing");
      delay(1);
    }
    statusInt = drawSurroundings();
    scanStarted = false;
  }

}

int drawSurroundings()
{

 //Get values and convert them to coordinants.
 float drawTable[getTableLength()][2];
 
 for(int i=0; i<getTableLength();i++)
 {
    int drawAngle=getAngleFromTable(i);
    int drawDistance=getDistanceFromTable(i);
    int kTest = 4;//If need scaling up
    drawTable[i][0]=get_X(float(drawDistance*kTest), float(drawAngle));
    drawTable[i][1]=get_Y(float(drawDistance*kTest), float(drawAngle));

    if(testingMode)
    {
      Serial.print("drawTable[i][0]: ");
      Serial.println(drawTable[i][0]);
    }
 }
 //testDrawing
 Serial.print("table Length is ");
 Serial.println(getTableLength());

//theory. send all with one contact? Madness?
establishContact(1);
Serial.println(map(drawTable[0][0],0,400,10,350)); // TrashValue

 for(int i=0; i<getTableLength(); i++)
 {
   waitingWork();
   Serial.println(map(drawTable[i][0],0,400,10,350)); // x
   waitingWork();
   Serial.println(map(drawTable[i][1],0,400,10,350)); // y
   //waitingWork();
   //Serial.println(map(drawTable[i+1][0],0,400,10,350)); // x
   //waitingWork();
   //Serial.println(map(drawTable[i+1][1],0,400,10,350)); // y
 }
 //draw the last line
 //waitingWork();
 //Serial.println(map(drawTable[0][0],0,400,0,400)); //start x
 //waitingWork();
 //Serial.println(map(drawTable[0][1],0,400,0,400)); //start y
 //waitingWork();
 //Serial.println(map(drawTable[getTableLength()-1][0],0,400,0,400));
 //waitingWork();
 //Serial.println(map(drawTable[getTableLength()-1][1],0,400,0,400));

 waitingWork();
 Serial.println(map(400,0,400,0,400)); //command to stop drawing
 //should also draw the last line
 
 //Serial.println(2); //command to clear screen
  return 0;
}

void detectStartButton()
{
  if(!digitalRead(START_BUTTON) && !startButtonPressed)
  {
    startButtonPressed =true;
    buttonDone = false;
  }
  else if(startButtonPressed && digitalRead(START_BUTTON))
  {
    startButtonPressed = false;
    buttonDone = true;
  }
}

float get_X(float drawDistance, float drawAngle)
{
  float fX= 10;
  //figure out X
  if(drawAngle==360 || drawAngle==180)
  {
    fX = 200;
  }
  else if(0 <drawAngle || drawAngle <90)
  {
    fX = 200+drawDistance*sin(drawAngle*PI/180.0)/2.0;
  }
  else if(90<drawAngle||drawAngle<180)
  {
    fX = 200+drawDistance*sin((180-drawAngle)*PI/180.0)/2.0;
  }
  else if(drawAngle ==90)
  {
    fX= 200+drawDistance/2;
  }
  else if(drawAngle==270)
  {
    fX = 200-drawDistance/2;
  }
  else if(180<drawAngle||drawAngle<270)
  {
    fX= 200-drawDistance*sin((drawAngle-180)*PI/180.0)/2.0;
  }
  else if(270<drawAngle||drawAngle<360)
  {
    fX= 200-drawDistance*sin((360-drawAngle)*PI/180.0)/2.0;
  }
  else
  {
    fX=200;
  }
  return fX;
}

float get_Y(float drawDistance, float drawAngle)
{
  float fY=10;
  //figure out Y
  if(drawAngle==90 || drawAngle==270)
  {
    fY = 200;
  }
  else if(0 <drawAngle||drawAngle <90)
  {
    fY = 200+drawDistance*cos(drawAngle*PI/180)/2;
  }
  else if(90<drawAngle||drawAngle<180)
  {
    fY = 200-drawDistance*cos((180-drawAngle)*PI/180)/2;
  }
  else if(drawAngle ==0)
  {
    fY= 200+drawDistance/2;
  }
  else if(drawAngle==180)
  {
    fY = 200-drawDistance/2;
  }
  else if(180<drawAngle||drawAngle<270)
  {
    fY= 200-drawDistance*cos((drawAngle-180)*PI/180)/2;
  }
  else if(270<drawAngle||drawAngle<360)
  {
    fY= 200+drawDistance*cos((360-drawAngle)*PI/180)/2;
  }
  else
  {
    fY=200;
  }
  return fY;
}

void waitingWork()
{
  waitProcessDraw=true;
  while(waitProcessDraw)
  {
      delay(10);
    int signInt = Serial.read();
    if(signInt=='A')
    {
      waitProcessDraw = false; //stop waiting and send
    }
  }
}

void establishContact(int message)
{
  boolean noContact=true;
   
  while(noContact)
  {
    Serial.write(message); 
    delay(300);
    int signInt = Serial.read();
    delay(10);
    if(signInt == 'A')
    {
      noContact=false; //send something
    }
  }
}
