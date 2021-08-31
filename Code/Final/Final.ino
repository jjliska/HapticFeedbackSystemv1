/*  Joshua Liska
 *  Project 1
 *  ------------------------------------------------------------------------------------------
 *  References:
 *    - My previous Buttons.ino assignment
 *    - Inspiration and building know how: https://www.youtube.com/watch?v=0uPZwMg5B3k
 *  ------------------------------------------------------------------------------------------
 *  Use Case:
 *    When the machineState is high then you can press on the "cup" to output a string that could potentially 
 *    be used by unity or some other program to create a 3d data point on where people grab the cup. This can be
 *    used to create a better future that creates more dynamic cup shapes for users with impaired movement/grip
 *    strength.
 */
//Row and collumn decleration (Used by everything)
const int colNum = 3;
const int rowNum = 2;

//Pins
const int cols[colNum] = {41,40,39};
const int rows[rowNum] = {15,14};

const int motorPin[colNum][rowNum] = {{2,3},
                                      {4,5},
                                      {6,7}};

//Variable Decleration
//Arrays
float pressureMatrix[colNum][rowNum];

float calibrationMatrix[colNum][rowNum];

//Timers
unsigned long printMatrixTimer = millis();

//Delays
const int delayTime = 100;

//Calibration Sample
const int sampleNum = 100;

bool machineState = false;

int pressureSensitivity = 100;

/*  void setup()
 *    sets up the matrixs and calibrates the calibration matrix
 */
void setup() {
  //pinMode decleartion
  for (int i = 0; i < colNum; i++) pinMode(cols[i], OUTPUT);
  for (int i = 0; i < rowNum; i++) pinMode(rows[i], INPUT);

  for(int i=0;i<colNum;i++){
    for(int j=0;j<rowNum;j++){
      pinMode(motorPin[i][j],OUTPUT);
    }
  }
  
  calibrateMatrix();
  printMatrix(true);
  delay(100);
}

/*  void loop()
 *    every second it takes a poll to read the matrix from the machine and outputs a string for another program to read
 */
void loop(){
  if(Serial.available() > 0){
    String serialIn = Serial.readStringUntil("\n");
    pressureSensitivity = serialIn.toInt();
  }
  
  if ((millis() - printMatrixTimer) >= delayTime) {
    readMatrix(false);
    //Comment out whichever you want one to show its working and the other to parse the string to unity/etc.
    printMatrix(false);
    motorPWM();
    //Serial.print("Parsed string:");Serial.println(stringParse());
    printMatrixTimer = millis();
  }
}

/*  void printMatrix(bool)
 *    Prints the matrix for debugging info
 */
void printMatrix(bool calib) {
  Serial.println("--------------------------------------------------------");
  for (int i = 0; i < rowNum; i++) {
    for (int j = 0; j < colNum; j++) {
      Serial.print("[ ");
      if (!calib) Serial.print(pressureMatrix[j][i]);
      else Serial.print(calibrationMatrix[j][i]);
      Serial.print(" ]");
    }
    Serial.println();
  }
  Serial.println("--------------------------------------------------------");
}

int preI = 0;

/*  void readMatrixc(bool)
 *    reads the matrix and determines if the run is a calibration run or not
 */
void readMatrix(bool calib) {
  int analogIn;
  for (int i = 0; i < colNum; i++) {
    digitalWrite(cols[preI], LOW);
    digitalWrite(cols[i], HIGH);
    preI = i;
    for (int j = 0; j < rowNum; j++) {
      analogIn = analogRead(rows[j]);
      if (!calib) {
        if ((analogIn - calibrationMatrix[i][j]) <= 0.0) pressureMatrix[i][j] = 0.0;
        else pressureMatrix[i][j] = float(analogIn) - calibrationMatrix[i][j];
      }
      else calibrationMatrix[i][j] = calibrationMatrix[i][j] + (float(analogIn) / sampleNum);
    }
  }
}

/*  void calibrateMatrix()
 *    Calibrates the matrix given 100 inputs at the start
 */
void calibrateMatrix(){
  //Init array
  for(int i = 0;i < colNum; i++) {
    for(int j = 0; j < rowNum; j++) {
      calibrationMatrix[i][j] = 0.0;
    }
  }
  for(int i = 0;i < sampleNum; i++) {
    readMatrix(true);
  }
  Serial.println("Calibrated matrix: ");
  //printMatrix(true);
}

/*  void motorPWM()
 *    Takes given input from 
 */
 void motorPWM(){
  Serial.println(pressureSensitivity);
  for(int i=0;i<rowNum;i++){
    for(int j=0;j<colNum;j++){
      int tempPWM = map(pressureMatrix[j][i],0,1023,0,127);
      float sensePWM = 0;
      if(tempPWM < 10) sensePWM = 0;
      else sensePWM = 128 + tempPWM * (float(pressureSensitivity)/100.0);
      Serial.print("[ ");Serial.print(sensePWM);Serial.print("]");
      analogWrite(motorPin[j][i], sensePWM);
    }
    Serial.println();
  }
 }
