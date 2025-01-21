PROJECT: Device that rotates ultrasound sensor. Measures distances from different angles and draws picture based on data.

Devices: Arduino UNO
Sensors: Ultrasound sensor HC-SR04, Gyro sensor GY-61 
Motor: Motor is 1:220 rotation worm drive motor, with working voltage from 1,5V to 12V DC, with rotation strength from 0.5 to 5kg/cm
Other: Modified Plastic Candybox

Softwares: Arduino IDE, Processing

All included code is made by me
Device was assembled by me
Candybox was modified by me

PROGRAM Description:
When Start button is pressed, device starts scanning.

First ultrasound value is angle 0 and it is saved as first point.

After that ultrasound should turn to next angle and take next measurement.

At third measurement ultrasound compares points based on distance and angle.
If all three points are same line new point is saved as old point, device moves to next angle.
If they are in different line, the old point is added as corner to tables. old point is saved as startPoint and new point is saves as oldpoint

Idea is that when corners are send to Processing, there is no 360 points but instead only points that matter in drawing.

After turning to 360-advance angle degrees, the device rotates back to starting angle.

Collected data is sent to Processing and picture is drawn.

Note: Program has button interruption with idea, that reset button could have been used to stop scan or clean picture.
Button interuption might have caused some issues in code and code might be better without it.

PROJECT ISSUES:
ULTRASOUND is not accurate enough if there are angled walls or corners pointing towards sensor.
Echo will bounce and give longer measurement, than the actual distance.
Also issue with corner near by can cause sensor give shorter measure than is right infront of it.
Echo does not bounce back from fabrics.

GYRO issues with steady values. Could be caused by button interruption.

MOTOR:
When using Arduino UNO as source ouf voltage, motor is not strong enough to turn properly and causes issues with getting the device turn accurately.
Button interruption might have also caused issues with this.

FUTURE SUGGESTION:
Remove button interruption and recalibrate everything.

In Picture there are pictures I was able to draw with the Device
![image](https://github.com/user-attachments/assets/424ce886-2ab7-4963-b90f-c844b71959b8)

Device: Ultrasoundrenderer and next to it broken Motor I broke when I tried to attach it to device first time. I had to buy another one.
![image](https://github.com/user-attachments/assets/4b70be5c-d0ac-43af-98e3-25c279a10b66)
