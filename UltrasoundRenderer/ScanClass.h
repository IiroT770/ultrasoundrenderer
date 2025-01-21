//This is what will be the public info of the class
#ifndef ScanClass_h
#define ScanClass_h

//include if needed something

#endif

int scanProgram();
void startSerial(boolean testSerialko);
int getPointFromTable(int getPointID);
int getAngleFromTable(int getAngleID);
int getDistanceFromTable(int getDistanceID);
int getTableLength();

//Test functions
void testGyroRotation();
void testRotation();
