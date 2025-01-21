#define CLOCK_WISE_PIN 6
#define COUNTER_WISE_PIN 5
//Scan environment
#include "Arduino.h"
#include "UltraClass.h"

boolean testingSerial = false;

//Gyro Y is white wire in circuit
//y-Pin is A2
const int GYRO_Y_PIN = A2;
volatile int gyroResult = 0;

int startPointID = 0;
volatile float currentAngle = 0.0;
int scanDistance = 0;
const int angleStep = 10; //degrees moved

int angleMistake = 1; //how accurate we assume the measurement to be?
int bSideMistake = 10; //clearly bigger than assumed... how much bigger?
volatile unsigned long turnTimerLong = 0;
unsigned long minTurnDelay = 10;


int tableLength = 1;
int distanceTable[36];
int angleTable[36];
//36 points if measuring every 10 degrees.

void addPointToTable(int pointID, int pointDistance, int pointAngle); //tableName[rows][columns]
void rotateAngle(int turnAngle); //turn ClockWise
void resetRotationAngle();//return to angle 0 CounterWise
void testPoints(int testDistance);
//Tests if new line, adds new line if there is one
//other ways continues old line
void turnClockWise();
void turnCounterWise();
void stopRotation();
void countNewAngleClock();
void countNewAngleCounter();
int getPointFromTable(int getPointID);

void startSerial(boolean testSerialko)
{
  Serial.begin(9600);
  pinMode(CLOCK_WISE_PIN,OUTPUT);
  digitalWrite(CLOCK_WISE_PIN, 0); //testing connections
  
  pinMode(COUNTER_WISE_PIN,OUTPUT);
  digitalWrite(COUNTER_WISE_PIN, 0);

  testingSerial = testSerialko;
  
  turnTimerLong = millis();
}


//issue 1
//Ultrasound seems to get higher numbers if hitting corner, that devides the echo.
//Biggest measurements like 1194 seem to happen when
//ultrasound is pointing at soffa, clothes or anything that
//might muffle the signal out

//Consider in code the upper qualities.

// make rotation and angle measurement before issue 1

int scanProgram()
{
  startPointID = 0;
  addPointToTable(0, measureDistance(), 360); //add first point always
  tableLength = 1;
  
  if(testingSerial)
  {
    Serial.println("Scan Program started");
    delay(1000);
  }
  //Put second point in table too
  rotateAngle(angleStep);
    delay(1000);//delay after rotate for accuracy
  addPointToTable(1,measureDistance(), 360-int(currentAngle+0.5));//reverse the angle

  //Since first two first steps are measured, start from step three
  //which is AngleStep*2
  for(int iAngle= 2*angleStep; iAngle<360; iAngle = iAngle+angleStep)
  {
    //As long as long as full rotation have not been made
    //continue collecting new points
    
    rotateAngle(iAngle);
    delay(1000);
    scanDistance = measureDistance();

    testPoints(scanDistance); //with currentAngle
  }

  if(testingSerial)
  {
    Serial.print("currentAngle before Rotating back to zero: ");
    Serial.println(int(currentAngle+0.5));
  } 
  //test delay
  //delay(6000);
  resetRotationAngle(); //rotate back to zero degrees


  return 2;
}

void rotateAngle(int targetAngle) //aim for targetAngle
{
  if(testingSerial)
  {
    Serial.print("rotateAngle started with targetAngle: ");
    Serial.println(targetAngle);
  }
  while( targetAngle != (int(currentAngle+0.5)))
  {
    if(targetAngle > (int(currentAngle+1.0))) //if Target bigger than current, turn CLOCK WISE
    {
      if(testingSerial)
      {
        Serial.println("target is bigger than currentAngle");
      }
      
      turnClockWise(); //timer also started in this
      
      while(targetAngle > (int(currentAngle)+1.0)) //while target is bigger than current, continue
      {
        countNewAngleClock(); //Turn new currentAngle
      }
    }
    else if( targetAngle<(int(currentAngle))) //targetAngle tinier than currentAngle, backtrack
    {
      if(testingSerial)
      {
        Serial.println("target is tinier than currentAngle");
      }
      
      turnCounterWise();
      while ( targetAngle<(int(currentAngle)))
      {
        countNewAngleCounter();
      }
    }
    else
    {
      //if target is not enough big or tiny it must be good
      if(testingSerial)
      {
        Serial.println("target was not enough big or tiny");
      }
      stopRotation();
      break; //get out of the while loop
    }

    if(testingSerial)
    {
      Serial.print("currentAngle: ");
      Serial.println(int(currentAngle+0.5));
    }

    if((targetAngle-angleMistake) <= int(currentAngle+0.5)&& int(currentAngle+0.5)<= (targetAngle+angleMistake))
    {
      //Close enough, escape
      stopRotation();
      break; 
    }
  } //end of while
  
  stopRotation(); //just making sure that rotation stops. It should have stopped
} //end rotateAngle

void resetRotationAngle()
{
  //just return to 0 degrees
  int targetAngle = 0;
  
  turnCounterWise();
  while ( targetAngle<(int(currentAngle))) //while current Angle bigger than 0
  {
    countNewAngleCounter(); //towards zero
  }

  //if currentAngle is tiny enough, just stop
  stopRotation();
}

void countNewAngleClock()
{
  //Get rotation value and count new angle
  gyroResult = analogRead(GYRO_Y_PIN);
  unsigned long runTime = millis()-turnTimerLong;
  currentAngle = currentAngle + float(sqrt(abs(gyroResult-345.5)))*0.333253*runTime/1000.0*180.0/PI;
  turnTimerLong=millis(); //runTimer used so reset
}

void countNewAngleCounter()
{
  //wait little before counting angle
  delay(10);
  //Get rotation value and count new angle
  gyroResult = analogRead(GYRO_Y_PIN);
  unsigned long runTime = millis()-turnTimerLong;
  currentAngle = currentAngle - float(sqrt(abs(gyroResult-345.5)))*0.610304651*runTime/1000.0*180.0/PI;
  turnTimerLong=millis();
}

void turnClockWise()
{
  if(testingSerial)
  {
    Serial.println("Start turning CLOCKWISE");
  }
  digitalWrite(COUNTER_WISE_PIN, 0);
  delay(25);
  digitalWrite(CLOCK_WISE_PIN, 255);
  turnTimerLong = millis();
  //delay(75);
}

void turnCounterWise()
{
  if(testingSerial)
  {
    Serial.println("Start turning COUNTERWISE");
  }
  digitalWrite(CLOCK_WISE_PIN, 0);
  delay(25);
  digitalWrite(COUNTER_WISE_PIN, 255);
  turnTimerLong = millis();
  //delay(75);
}

void stopRotation()
{
  digitalWrite(COUNTER_WISE_PIN, 0);
  digitalWrite(CLOCK_WISE_PIN, 0);
  turnTimerLong = millis();
}

void testPoints(int testDistance)//use current Angle
{
  //Test if points are in same line or not
  int startTestDistance, startTestAngle = getPointFromTable(tableLength-2);
  int endTestDistance, endTestAngle = getPointFromTable(tableLength-1);
  
  int endStartAngle =abs( endTestAngle-startTestAngle);
  int aSideTest = endTestDistance*sin(endStartAngle*PI/180);
  int oldBSideTest = startTestDistance - endTestDistance*cos(endStartAngle*PI/180);
  float testSameAngle = atan(abs(aSideTest/oldBSideTest)); //do they share this Angle?

  int newTestAngle = abs((360-int(currentAngle+0.5)) - startTestAngle); //need reverse angle
  aSideTest = testDistance*sin(newTestAngle*PI/180);
  int newBSideTest = startTestDistance-testDistance*cos(newTestAngle*PI/180);
  float otherSameAngle = atan(abs(aSideTest/newBSideTest));

  float ifAngleMistake = 0.7853982;
  if((otherSameAngle +ifAngleMistake) > testSameAngle&& testSameAngle > (otherSameAngle - ifAngleMistake))
  {
    //New point is on old line and becomes new oldPoint
    addPointToTable(tableLength-1, testDistance, (360-int(currentAngle+0.5)));
  }
  else
  {
    //not on line, new Point found?
    addPointToTable(tableLength, testDistance, (360-int(currentAngle+0.5))); //new point
    
  }  
}

//Just store new points
void addPointToTable(int pointID, int pointDistance, int pointAngle)
{
  distanceTable[pointID] = pointDistance;
  angleTable[pointID] = pointAngle;
  
  if(tableLength == pointID)
  {
    //Point is new. tableLength grows.
    tableLength++;
  }
}

int getTableLength()
{
  return tableLength;
}

int getDistanceFromTable(int getDistanceID)
{
  return distanceTable[getDistanceID];
}
int getAngleFromTable(int getAngleID)
{
  return angleTable[getAngleID];
}
int getPointFromTable(int getPointID)
{
  return distanceTable[getPointID], angleTable[getPointID];
}




////////////////////////////////////////////
/////Test functions//////
/*
void testGyroRotation()
{
  //CLOCK WISE
  digitalWrite(CLOCK_WISE_PIN, 255);
  turnTimerLong = millis();
  unsigned long endTime = millis();
  
  for(int i = 0; i<10;i++)
  {
  gyroResult = analogRead(GYRO_X_PIN);
  endTime = millis() - turnTimerLong;
  Serial.print(endTime);
  Serial.print(" X_PIN : ");
  Serial.println(gyroResult);
  
  gyroResult = analogRead(GYRO_Y_PIN);
  endTime = millis() - turnTimerLong;
  Serial.print(endTime);
  Serial.print("Y_PIN : ");
  Serial.println(gyroResult);

  gyroResult = analogRead(GYRO_Z_PIN);
  endTime = millis() - turnTimerLong;
  Serial.print(endTime);
  Serial.print("Z_PIN : ");
  Serial.println(gyroResult);
  }
  digitalWrite(CLOCK_WISE_PIN, 0);
  Serial.print("CLOCK end");
  delay(6000);

  //COUNTER WISE
  digitalWrite(COUNTER_WISE_PIN,255);
  turnTimerLong = millis();

   for(int i = 0; i<10;i++)
  {
  gyroResult = analogRead(GYRO_X_PIN);
  endTime = millis() - turnTimerLong;
  Serial.print(endTime);
  Serial.print(" X_PIN : ");
  Serial.println(gyroResult);
  
  gyroResult = analogRead(GYRO_Y_PIN);
  endTime = millis() - turnTimerLong;
  Serial.print(endTime);
  Serial.print("Y_PIN : ");
  Serial.println(gyroResult);

  gyroResult = analogRead(GYRO_Z_PIN);
  endTime = millis() - turnTimerLong;
  Serial.print(endTime);
  Serial.print("Z_PIN : ");
  Serial.println(gyroResult);
  }
  digitalWrite(COUNTER_WISE_PIN, 0);
  Serial.print("COUNTER END");
  Serial.println(endTime);
  delay(5000);
}
//result
//Squareroot(y-342.5)* k * deltaTime(s) = rad Angle
//CLOCK k = 0.632218018
//COUNTER k = 0.77317801


void testRotation()
{
  //CLOCK WISE
  for(int i = 0; i<35; i++)
  {
    digitalWrite(CLOCK_WISE_PIN, 255);
    delay(112);//about 10 degree turn
    digitalWrite(CLOCK_WISE_PIN,0);
    delay(1000);
  }
  delay(5000);
  //COUNTER WISE
  digitalWrite(COUNTER_WISE_PIN, 255);
  delay(2965); //360degree turn
  digitalWrite(COUNTER_WISE_PIN,0);
  delay(5000);
}
*/
