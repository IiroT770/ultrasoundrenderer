import processing.serial.*;

Serial myPort;  // Create object from Serial class
static String val;    // Data received from the serial port
int sensorVal[] = {0,0,0,0,0};
int xStart, yStart, xEnd, yEnd = 0;
int commandVal= 0; 
int sensorValID = 0;
//0 = do nothing
//1 = draw
//2 = clear

//Serial communication
volatile boolean firstContact = false;

void setup()
{
  size(400, 400);
  noSmooth();
  background(0);
  //translate(140, 0);
  String portName = "COM3";// Change the number (in this case ) to match the corresponding port number connected to your Arduino. 

  myPort = new Serial(this, portName, 9600);
}

void draw()
{
  if(commandVal==1)
  {
  stroke(153);
  line(xStart,yStart,xEnd,yEnd);
  commandVal=0;
  }
  else if(commandVal==2)
  {
    background(0);
    firstContact = false;
    commandVal=0;
    xEnd=0;
  }
}

void serialEvent(Serial myPort) {
  // read a line from the serial port:
  int testVal=0;
  // if this is the first line and it is 1 or 2
  // clear the serial buffer and note that you've
  // had first contact from the microcontroller. 
  if (firstContact == false) {
    testVal= myPort.read(); 
    if (testVal == 1) { 
      myPort.clear();          // clear the serial port buffer
      firstContact = true;     // you've had first contact from the microcontroller
      myPort.write('A');  // ask for more
    }
    else if(testVal ==2)
    {
      commandVal = 2;
    }
  } 
  else {
    if ( myPort.available() > 0) { 
      val=myPort.readStringUntil('\n');
    
      try {
       testVal = Integer.valueOf(val.trim());
      }
      catch(Exception e) {
      ;
      }
      // Add the latest byte from the serial port to array:
      if((testVal>3) && (testVal<400))
      {
        sensorVal[sensorValID]=testVal;
        sensorValID++;     
      }
      else if(testVal==400)//command to stop drawing
      {
        xStart = sensorVal[1];
        yStart= sensorVal[2];
        commandVal=1; //draw last line and stop
        sensorValID =0;
        firstContact=true;
      }

      if(sensorValID==3 && xEnd ==0)//xEnd should not be 0 after this
      {
        //First two values achieved
        xEnd = sensorVal[1];
        yEnd= sensorVal[2];
      }
      if(sensorValID==5)
      {
        //all sensor Values are here
        xStart = xEnd;
        yStart = yEnd;
        xEnd = sensorVal[3];
        yEnd = sensorVal[4];
        sensorValID=3;
        commandVal=1; //draw
      }
    }
    // Send a capital A to request new sensor readings:
    myPort.write('A');
  }
}
